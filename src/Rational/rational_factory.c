/*
 * rational_factory.c
 *
 * Factory methods for OceanMoon\Math\Rational: fromFloat(), fromString(). Mirrors the "Factory
 * methods" region of the PHP package's Rational class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <math.h>
#include <string.h>

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "rational_internal.h"
#include "../../oceanmoon_math_arginfo.h"
#include "exceptions.h"
#include "floats.h"
#include "integers.h"

/* {{{ rational_cf_terms
 *
 * Matches the PHP package's Rational::cfTerms(): the terms of the continued fraction of x's exact value, using integer
 * arithmetic only. x is M / 2^s, with M odd and less than 2^53, so the terms come from Euclid's algorithm. 2^s is too
 * large for a zend_long when s > 62, which only happens when x is less than 2^-9. Its first term is then 0, and the
 * second, floor(2^s / M), is found one bit at a time, with the remainder. The rest only involve values below 2^53.
 *
 * x must be positive, finite, not an integer, and at least 2^-63. Returns the number of terms written to terms, which
 * has room for RATIONAL_CF_MAX_TERMS: Euclid's algorithm on values below 2^63 takes at most 92 steps.
 */
#define RATIONAL_CF_MAX_TERMS 128

static zend_long rational_cf_terms(double x, zend_long *terms)
{
	/* Get M and s, with x = M / 2^s. A double in range is normal, so its significand has the implicit leading 1. */
	uint64_t bits;
	memcpy(&bits, &x, sizeof bits);
	zend_long m = (zend_long) ((bits & 0xFFFFFFFFFFFFFULL) | (1ULL << 52));
	zend_long s = 1075 - (zend_long) ((bits >> 52) & 0x7FF);
	while ((m & 1) == 0) {
		m >>= 1;
		s--;
	}

	zend_long count = 0;
	zend_long p, q;
	if (s <= 62) {
		p = m;
		q = (zend_long) 1 << s;
	} else {
		zend_long a = 0, r = 1;
		for (zend_long i = 0; i < s; i++) {
			a += a;
			r += r;
			if (r >= m) {
				r -= m;
				a++;
			}
		}
		terms[count++] = 0;
		terms[count++] = a;
		p = m;
		q = r;
	}

	/* Euclid's algorithm. */
	while (q != 0 && count < RATIONAL_CF_MAX_TERMS) {
		zend_long a = p / q;
		terms[count++] = a;
		zend_long rem = p - a * q;
		p = q;
		q = rem;
	}

	return count;
}
/* }}} */

/* {{{ rational_float_value_is
 *
 * Matches the PHP package's Rational::floatValueIs(): whether num/den converts to x, as toFloat() would convert it.
 */
static bool rational_float_value_is(zend_long num, zend_long den, double x)
{
	return (double) num / (double) den == x;
}
/* }}} */

/* {{{ rational_simplest_semiconvergent
 *
 * Matches the PHP package's Rational::simplestSemiconvergent(): the semiconvergent with the smallest t from 1 to
 * t_max, (h1 + t * h0) / (k1 + t * k0), whose float value is x. The one with t_max must be. As t increases, the
 * semiconvergents approach x from one side, so a binary search finds the first.
 */
static void rational_simplest_semiconvergent(
	zend_long h0, zend_long k0, zend_long h1, zend_long k1, zend_long t_max, double x, zend_long *num, zend_long *den
)
{
	zend_long low = 1, high = t_max;
	while (low < high) {
		zend_long mid = low + (high - low) / 2;
		if (rational_float_value_is(h1 + mid * h0, k1 + mid * k0, x)) {
			high = mid;
		} else {
			low = mid + 1;
		}
	}
	*num = h1 + high * h0;
	*den = k1 + high * k0;
}
/* }}} */

/* {{{ rational_semiconvergent_is_closer
 *
 * Matches the PHP package's Rational::semiconvergentIsCloser(): whether the semiconvergent with t is closer to x than
 * the convergent before term n. With a the nth term, it is if t > a/2, and isn't if t < a/2. If t = a/2, it is if
 * [a(n+1); a(n+2), ...] > [a(n-1); a(n-2), ..., a(1)], compared by Rational::compareContinuedFractions()'s rule: the
 * first differing term decides it, with a term past the end being infinity, and the order reversing at odd positions.
 * A trailing term of 1 in the second is folded into the one before, so equal values compare as equal.
 */
static bool rational_semiconvergent_is_closer(const zend_long *terms, zend_long count, zend_long n, zend_long t)
{
	zend_long a = terms[n];
	if (t != a - t) {
		return t > a - t;
	}

	/* rest[i] is terms[n + 1 + i]. ratio[i] is terms[n - 1 - i], for i up to n - 2, with the fold applied. */
	zend_long rest_len = count - n - 1;
	zend_long ratio_len = n - 1;
	bool folded = ratio_len > 1 && terms[1] == 1;
	if (folded) {
		ratio_len--;
	}

	zend_long len = rest_len > ratio_len ? rest_len : ratio_len;
	for (zend_long i = 0; i < len; i++) {
		bool rest_inf = i >= rest_len;
		bool ratio_inf = i >= ratio_len;
		zend_long rest_term = rest_inf ? 0 : terms[n + 1 + i];
		zend_long ratio_term = ratio_inf ? 0 : (folded && i == ratio_len - 1 ? terms[2] + 1 : terms[n - 1 - i]);
		if (rest_inf != ratio_inf || rest_term != ratio_term) {
			int cmp = rest_inf ? 1 : (ratio_inf ? -1 : (rest_term > ratio_term ? 1 : -1));
			return (i % 2 == 0 ? cmp : -cmp) > 0;
		}
	}
	return false;
}
/* }}} */

/* {{{ rational_approximate_float
 *
 * Matches the PHP package's Rational::approximateFloat(): the simplest fraction num/den, with both in the zend_long
 * range, whose float value is x, or the closest if there's none. See its doc comment for the algorithm.
 */
static void rational_approximate_float(double x, zend_long *num, zend_long *den)
{
	zend_long terms[RATIONAL_CF_MAX_TERMS];
	zend_long count = rational_cf_terms(x, terms);

	/* The two most recent convergents, h0/k0 and h1/k1. Initially, the convergents before the first: 1/0 and 0/1. */
	zend_long h0 = 1, k0 = 0, h1 = 0, k1 = 1;

	for (zend_long n = 0; n < count; n++) {
		zend_long a = terms[n];
		zend_long h, k, tmp;
		bool overflow = math_integers_mul(a, h0, &tmp) != MATH_INTEGERS_OK
			|| math_integers_add(tmp, h1, &h) != MATH_INTEGERS_OK
			|| math_integers_mul(a, k0, &tmp) != MATH_INTEGERS_OK
			|| math_integers_add(tmp, k1, &k) != MATH_INTEGERS_OK;

		/* If the next convergent overflows, try the best semiconvergent that fits: the largest t such that
		 * (h1 + t * h0) / (k1 + t * k0) is in range. It's less than a, as t = a gives the convergent. */
		if (overflow) {
			zend_long t = a - 1;
			if (k0 != 0 && (ZEND_LONG_MAX - k1) / k0 < t) {
				t = (ZEND_LONG_MAX - k1) / k0;
			}
			if (h0 != 0 && (ZEND_LONG_MAX - h1) / h0 < t) {
				t = (ZEND_LONG_MAX - h1) / h0;
			}
			if (t >= 1) {
				if (rational_float_value_is(h1 + t * h0, k1 + t * k0, x)) {
					rational_simplest_semiconvergent(h0, k0, h1, k1, t, x, num, den);
					return;
				}
				if (rational_semiconvergent_is_closer(terms, count, n, t)) {
					*num = h1 + t * h0;
					*den = k1 + t * k0;
					return;
				}
			}
			*num = h0;
			*den = k0;
			return;
		}

		if (rational_float_value_is(h, k, x)) {
			rational_simplest_semiconvergent(h0, k0, h1, k1, a, x, num, den);
			return;
		}

		h1 = h0;
		k1 = k0;
		h0 = h;
		k0 = k;
	}

	/* Not reached: the last convergent is x exactly, and so converts back to it. */
	*num = h0;
	*den = k0;
}
/* }}} */

/* {{{ rational_calc_from_float
 *
 * The computational core of fromFloat(), factored out so fromString() (below) can call it
 * directly for its "looks like a float" branch, without going through a full PHP method dispatch.
 *
 * Matches the PHP package's Rational::fromFloat() exactly: a whole number converts directly, and anything else goes
 * through rational_approximate_float().
 */
zend_result rational_calc_from_float(double value, zval *return_value)
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
	 * itself (`i > ZEND_LONG_MIN`, not `>=`) - rational_create(ZEND_LONG_MIN, 1) would throw
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

	/* Find the simplest Rational that converts back to the float, or failing that, the closest. */
	zend_long num, den;
	rational_approximate_float(abs_value, &num, &den);
	return rational_create(return_value, sign_val * num, den);
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
 * Calls the real userland filter_var($str, $filter) - rather than reimplementing
 * FILTER_VALIDATE_INT's parsing rules in C - so fromString()'s int detection is guaranteed
 * byte-for-byte identical to the PHP package's, which uses the same function. $filter is looked up by its constant name each call rather than hard-coded, in case
 * its integer value ever changes (unlikely, but free to guard against).
 *
 * Returns FAILURE only if the FILTER_VALIDATE_* constant itself can't be found (ext/filter not
 * loaded) or the call itself fails - an unmatched value (filter_var() returning false) is still
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
 * Trims both ends of str with math_types_trim(), so the same characters are stripped as in the package. Returns a new
 * zend_string the caller owns.
 */
static zend_string *rational_trim(const char *str, size_t len)
{
	zend_string *untrimmed = zend_string_init(str, len, 0);
	zend_string *trimmed = math_types_trim(untrimmed);
	zend_string_release(untrimmed);
	return trimmed;
}
/* }}} */

/* {{{ rational_int_string_out_of_range
 *
 * Whether str is an integer, e.g. "-0042", too large in magnitude for a zend_long. Matches the package's check, which
 * uses the regex /^[+-]?0*(\d+)$/, then compares the digits with PHP_INT_MAX's, ignoring leading zeros.
 */
static bool rational_int_string_out_of_range(const char *str, size_t len)
{
	size_t start = 0;
	if (start < len && (str[start] == '+' || str[start] == '-')) {
		start++;
	}
	if (start == len) {
		return false;
	}
	for (size_t i = start; i < len; i++) {
		if (str[i] < '0' || str[i] > '9') {
			return false;
		}
	}

	/* Skip leading zeros, keeping at least one digit. */
	while (start < len - 1 && str[start] == '0') {
		start++;
	}

	char max[32];
	size_t max_len = (size_t) snprintf(max, sizeof(max), ZEND_LONG_FMT, ZEND_LONG_MAX);
	size_t digits = len - start;
	return digits > max_len || (digits == max_len && memcmp(str + start, max, max_len) > 0);
}
/* }}} */

/* {{{ rational_throw_string_out_of_range
 *
 * Throws DomainException "Cannot convert string '...' to Rational. Outside valid range.", with the string quoted and
 * abbreviated as the package's Stringify::prepEx() does. Any exception already thrown becomes its previous one.
 */
static void rational_throw_string_out_of_range(zend_string *str)
{
	math_throw_string_error(spl_ce_DomainException, ZSTR_VAL(str), ZSTR_LEN(str), "Rational", "Outside valid range.");
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::fromString(string $str): Rational
 *
 * Matches the PHP package's Rational::fromString(): accepts an int ("123"), a float ("123.456",
 * converted via fromFloat()), or a fraction ("1/2"). Delegates int recognition to the real
 * filter_var() (see rational_filter_var()'s doc comment for why), and float recognition to
 * is_numeric_string(), which backs the package's is_numeric().
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

	/* Check for an integer too large for a zend_long. FILTER_VALIDATE_INT rejects it, and as a float it would round,
	 * then be clamped to ZEND_LONG_MAX. */
	if (rational_int_string_out_of_range(ZSTR_VAL(trimmed), ZSTR_LEN(trimmed))) {
		rational_throw_string_out_of_range(trimmed);
		zend_string_release(trimmed);
		RETURN_THROWS();
	}

	/* Check for a string that looks like a float. Like the package's is_numeric(), is_numeric_string() accepts a
	 * number too large for a float, e.g. "1e400", which converts to INF, so it's reported as out of range, not as
	 * invalid. */
	zend_long lval;
	double dval;
	uint8_t type = is_numeric_string(ZSTR_VAL(trimmed), ZSTR_LEN(trimmed), &lval, &dval, false);
	if (type == IS_LONG || type == IS_DOUBLE) {
		if (rational_calc_from_float(type == IS_LONG ? (double) lval : dval, return_value) == FAILURE) {
			if (EG(exception) && instanceof_function(EG(exception)->ce, spl_ce_DomainException)) {
				rational_throw_string_out_of_range(trimmed);
			}
			zend_string_release(trimmed);
			RETURN_THROWS();
		}
		zend_string_release(trimmed);
		return;
	}

	/* Check for a string that looks like a fraction (int/int) - exactly one '/' character,
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

	math_throw_invalid_format(ZSTR_VAL(trimmed), ZSTR_LEN(trimmed), "Rational");
	zend_string_release(trimmed);
	RETURN_THROWS();
}
/* }}} */
