/*
 * rational_factory.c
 *
 * Factory methods for OceanMoon\Math\Rational: fromFloat(), fromString(). Mirrors the "Factory
 * methods" region of the PHP package's Rational class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <ctype.h>
#include <math.h>
#include <string.h>

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "rational_internal.h"
#include "rational_arginfo.h"
#include "exceptions.h"
#include "floats.h"
#include "integers.h"

/* {{{ rational_calc_from_float
 *
 * The computational core of fromFloat(), factored out so fromString() (below) can call it
 * directly for its "looks like a float" branch, without going through a full PHP method dispatch.
 *
 * Matches the PHP package's Rational::fromFloat() exactly, including its continued-fractions
 * approximation for non-whole-number values -- see the PHP source's own doc comment for the
 * algorithm reference. The accumulated numerator/denominator (h/k) are tracked via
 * math_integers_mul()/math_integers_add() instead of PHP's "let it silently promote to float and
 * check is_float()" overflow detection, since C has no such promotion (signed overflow is
 * undefined behavior, not an observable value) -- see integers.h.
 *
 * One additional guard has no PHP equivalent: if the working value `x` ever grows too large to
 * safely truncate to a zend_long, this bails out to the best approximation found so far, same as
 * the accumulator-overflow path. PHP's bare `(int) $x` cast has no such safeguard, but every
 * double has an exact, bounded-length continued-fraction expansion, so this is only a defensive
 * backstop against a case the algorithm's own real-world behavior never actually reaches (matched
 * by every input this package's test suite exercises, including its extreme-magnitude cases).
 */
static zend_result rational_calc_from_float(double value, zval *return_value)
{
	if (!zend_finite(value)) {
		zend_string *value_str = math_floats_to_str(value);
		zend_string *msg = strpprintf(
			0, "Cannot create Rational from non-finite float: %s.", ZSTR_VAL(value_str));
		zend_string_release(value_str);
		zend_throw_exception(spl_ce_DomainException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	/* Check if the value equals a valid integer. Deliberately excludes the ZEND_LONG_MIN result
	 * itself (`i > ZEND_LONG_MIN`, not `>=`) -- rational_create(ZEND_LONG_MIN, 1) would throw
	 * (denominator 1 is odd), so that exact value falls through to the general algorithm below,
	 * which handles it via the max-magnitude shortcut instead. */
	zend_long i;
	if (math_floats_to_int(value, &i) && i > ZEND_LONG_MIN) {
		return rational_create(return_value, i, 1);
	}

	double abs_value = fabs(value);
	/* value != 0 is guaranteed here (0 always takes the integer shortcut above), so no
	 * negative-zero case to consider. */
	int sign_val = value > 0 ? 1 : -1;
	double min = 1.0 / (double) ZEND_LONG_MAX;
	double max = (double) ZEND_LONG_MAX;

	if (abs_value < min || abs_value > max) {
		zend_string *value_str = math_floats_to_str(value);
		zend_string *msg = strpprintf(
			0, "Cannot create Rational from float: %s. Outside valid range.", ZSTR_VAL(value_str));
		zend_string_release(value_str);
		zend_throw_exception(spl_ce_DomainException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	/* Check for values at the limits of the valid range, which the continued-fractions algorithm
	 * below can't handle (dividing by an exactly-zero remainder at the first step). */
	if (abs_value == min) {
		return rational_create(return_value, sign_val, ZEND_LONG_MAX);
	}
	if (abs_value == max) {
		return rational_create(return_value, (zend_long) sign_val * ZEND_LONG_MAX, 1);
	}

	/* Use the continued fractions algorithm to convert the float to the closest possible
	 * Rational. Track the best approximation found so far, initialized to the nearest integer. */
	zend_long h_best = (zend_long) round(abs_value);
	zend_long k_best = 1;

	/* Initialize convergents. */
	zend_long h0 = 1, h1 = 0, k0 = 0, k1 = 1;

	double x = abs_value;
	double min_err = fabs((double) h_best - abs_value);

	while (true) {
		/* Defensive backstop: see doc comment above. */
		if (x >= (double) ZEND_LONG_MAX || x < (double) ZEND_LONG_MIN) {
			return rational_create(return_value, sign_val * h_best, k_best);
		}

		/* Extract integer part. */
		zend_long a = (zend_long) x;

		/* Calculate next convergent, with overflow detection. */
		zend_long h_new, k_new, tmp;
		bool overflow = math_integers_mul(a, h0, &tmp) != MATH_INTEGERS_OK
			|| math_integers_add(tmp, h1, &h_new) != MATH_INTEGERS_OK;
		if (!overflow) {
			overflow = math_integers_mul(a, k0, &tmp) != MATH_INTEGERS_OK
				|| math_integers_add(tmp, k1, &k_new) != MATH_INTEGERS_OK;
		}

		/* If the numerator or the denominator overflows the range for integers, cease the loop
		 * and return the best approximation found so far. */
		if (overflow) {
			return rational_create(return_value, sign_val * h_best, k_best);
		}

		/* Check if we've found an exact representation. */
		double err = fabs((double) h_new / (double) k_new - abs_value);
		if (err == 0.0) {
			return rational_create(return_value, sign_val * h_new, k_new);
		}

		/* Check if this convergent is better than the best so far. */
		if (err < min_err) {
			h_best = h_new;
			k_best = k_new;
			min_err = err;
		}

		/* Update convergents. */
		h1 = h0;
		h0 = h_new;
		k1 = k0;
		k0 = k_new;

		/* Calculate remainder. If it's 0, we're done. */
		double rem = x - (double) a;
		if (rem == 0.0) {
			return rational_create(return_value, sign_val * h0, k0);
		}

		/* Calculate next approximation. */
		x = 1.0 / rem;
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::fromFloat(float $value): Rational */
PHP_METHOD(OceanMoon_Math_Rational, fromFloat)
{
	double value;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(value)
	ZEND_PARSE_PARAMETERS_END();

	if (rational_calc_from_float(value, return_value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ rational_filter_var
 *
 * Calls the real userland filter_var($str, $filter) -- rather than reimplementing
 * FILTER_VALIDATE_INT/FILTER_VALIDATE_FLOAT's parsing rules in C -- so fromString()'s int/float
 * detection is guaranteed byte-for-byte identical to the PHP package's, which uses the same
 * function. $filter is looked up by its constant name each call rather than hard-coded, in case
 * its integer value ever changes (unlikely, but free to guard against).
 *
 * Returns FAILURE only if the FILTER_VALIDATE_* constant itself can't be found (ext/filter not
 * loaded) or the call itself fails -- an unmatched value (filter_var() returning false) is still
 * SUCCESS, with *out left as IS_FALSE for the caller to check.
 */
static zend_result rational_filter_var(zend_string *str, const char *filter_const_name, size_t filter_const_name_len, zval *out)
{
	zval *filter_const = zend_get_constant_str(filter_const_name, filter_const_name_len);
	if (filter_const == NULL) {
		return FAILURE;
	}

	zval func_name, params[2];
	ZVAL_STRING(&func_name, "filter_var");
	ZVAL_STR_COPY(&params[0], str);
	ZVAL_COPY_VALUE(&params[1], filter_const);

	zend_result result = call_user_function(NULL, NULL, &func_name, out, 2, params);

	zval_ptr_dtor(&func_name);
	zval_ptr_dtor(&params[0]);

	return result;
}
/* }}} */

/* {{{ rational_trim
 *
 * Trims ASCII whitespace from both ends of str, matching trim()'s default character set closely
 * enough for this file's purposes (fromString() only ever trims around digits/signs/slashes).
 * Returns a new zend_string the caller owns.
 */
static zend_string *rational_trim(const char *str, size_t len)
{
	size_t start = 0;
	while (start < len && isspace((unsigned char) str[start])) {
		start++;
	}
	size_t end = len;
	while (end > start && isspace((unsigned char) str[end - 1])) {
		end--;
	}
	return zend_string_init(str + start, end - start, 0);
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::fromString(string $str): Rational
 *
 * Matches the PHP package's Rational::fromString(): accepts an int ("123"), a float ("123.456",
 * converted via fromFloat()), or a fraction ("1/2"). Delegates the actual int/float recognition
 * to the real filter_var() (see rational_filter_var()'s doc comment for why).
 */
PHP_METHOD(OceanMoon_Math_Rational, fromString)
{
	zend_string *str;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(str)
	ZEND_PARSE_PARAMETERS_END();

	zend_string *trimmed = rational_trim(ZSTR_VAL(str), ZSTR_LEN(str));

	if (ZSTR_LEN(trimmed) == 0) {
		zend_string_release(trimmed);
		zend_throw_exception(math_ce_FormatException, "Cannot convert empty string to Rational.", 0);
		RETURN_THROWS();
	}

	/* Check for a string that looks like an integer. */
	zval filter_result;
	if (rational_filter_var(trimmed, ZEND_STRL("FILTER_VALIDATE_INT"), &filter_result) == SUCCESS
		&& Z_TYPE(filter_result) == IS_LONG) {
		zend_long n = Z_LVAL(filter_result);
		zend_string_release(trimmed);
		if (rational_create(return_value, n, 1) == FAILURE) {
			RETURN_THROWS();
		}
		return;
	}

	/* Check for a string that looks like a float. */
	if (rational_filter_var(trimmed, ZEND_STRL("FILTER_VALIDATE_FLOAT"), &filter_result) == SUCCESS
		&& Z_TYPE(filter_result) == IS_DOUBLE) {
		double n = Z_DVAL(filter_result);
		zend_string_release(trimmed);
		if (rational_calc_from_float(n, return_value) == FAILURE) {
			RETURN_THROWS();
		}
		return;
	}

	/* Check for a string that looks like a fraction (int/int) -- exactly one '/' character,
	 * matching explode('/', $str) having exactly 2 parts. */
	const char *start = ZSTR_VAL(trimmed);
	size_t len = ZSTR_LEN(trimmed);
	const char *slash = memchr(start, '/', len);

	if (slash != NULL && memchr(slash + 1, '/', (size_t) (start + len - (slash + 1))) == NULL) {
		zend_string *left = rational_trim(start, (size_t) (slash - start));
		zend_string *right = rational_trim(slash + 1, (size_t) (start + len - (slash + 1)));

		zval left_result, right_result;
		bool left_ok = rational_filter_var(left, ZEND_STRL("FILTER_VALIDATE_INT"), &left_result) == SUCCESS
			&& Z_TYPE(left_result) == IS_LONG;
		bool right_ok = rational_filter_var(right, ZEND_STRL("FILTER_VALIDATE_INT"), &right_result) == SUCCESS
			&& Z_TYPE(right_result) == IS_LONG;

		zend_string_release(left);
		zend_string_release(right);

		if (left_ok && right_ok) {
			zend_string_release(trimmed);
			if (rational_create(return_value, Z_LVAL(left_result), Z_LVAL(right_result)) == FAILURE) {
				RETURN_THROWS();
			}
			return;
		}
	}

	zend_string_release(trimmed);
	zend_throw_exception(math_ce_FormatException, "Cannot convert string to Rational. Invalid format.", 0);
	RETURN_THROWS();
}
/* }}} */
