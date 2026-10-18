/*
 * rational_comparison.c
 *
 * Comparison methods for OceanMoon\Math\Rational: compare(), approxEqual(), plus the methods the
 * PHP package's Rational class inherits from the ApproxComparable trait (equal(), lessThan(),
 * lessThanOrEqual(), greaterThan(), greaterThanOrEqual(), approxCompare()) - internal classes
 * can't use traits, so this file ports both the class's own methods and the trait's, matching the
 * "Comparison methods" region of the PHP package's Rational class plus
 * OceanMoon\Core\Traits\Comparison\Comparable/ApproxComparable.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <float.h>
#include <math.h>

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "rational_internal.h"
#include "../../oceanmoon_math_arginfo.h"
#include "floats.h"
#include "integers.h"
#include "types.h"

/* 2^53, the largest power of 2 below which every integer is exactly representable as a double. */
#define MATH_MAX_EXACT_INT ((zend_long) 1 << 53)

/* {{{ rational_next_binary_digit
 *
 * Matches the PHP package's Rational::nextBinaryDigit(): returns the next binary digit of the fraction rem/den, which
 * is less than 1, and updates *rem to the rest of it.
 */
static int rational_next_binary_digit(zend_long *rem, zend_long den)
{
	/* Double the fraction and take its whole number part, without calculating 2 * rem, which could overflow. */
	if (*rem >= den - *rem) {
		*rem -= den - *rem;
		return 1;
	}
	*rem += *rem;
	return 0;
}
/* }}} */

/* {{{ rational_compare_fractions
 *
 * Matches the PHP package's Rational::compareFractions(): compares two fractions in the range [0, 1) exactly, one
 * binary digit at a time. The first is rem/den. The second is other_rem/other_den, or, if other_frac isn't NULL,
 * *other_frac itself.
 *
 * This always ends. Two different fractions with denominators below 2^63 differ within 127 binary digits, and a float
 * fraction has at most 1074.
 */
static int rational_compare_fractions(
	zend_long rem, zend_long den, zend_long other_rem, zend_long other_den, const double *other_frac
)
{
	double frac = other_frac ? *other_frac : 0.0;
	while (true) {
		/* If either fraction has no more digits, the one with digits left is greater. */
		bool has_digits = rem > 0;
		bool other_has_digits = other_frac ? frac > 0 : other_rem > 0;
		if (!has_digits || !other_has_digits) {
			return (int) has_digits - (int) other_has_digits;
		}

		/* Get the next digit of each. */
		int digit = rational_next_binary_digit(&rem, den);
		int other_digit;
		if (other_frac) {
			/* Doubling a double is exact. */
			frac *= 2;
			other_digit = frac >= 1 ? 1 : 0;
			frac -= other_digit;
		} else {
			other_digit = rational_next_binary_digit(&other_rem, other_den);
		}

		if (digit != other_digit) {
			return digit - other_digit;
		}
	}
}
/* }}} */

/* {{{ rational_split_magnitude
 *
 * Splits the magnitude of num/den into a whole number part and a remainder, both non-negative - the magnitudes of the
 * parts the PHP package's Rational::compareExact() gets from toMixedNumber(). A numerator is never ZEND_LONG_MIN, as
 * it's in lowest terms, so negating either part is safe.
 */
static void rational_split_magnitude(zend_long num, zend_long den, zend_long *whole, zend_long *rem)
{
	zend_long q = num / den, r = num % den;
	*whole = q < 0 ? -q : q;
	*rem = r < 0 ? -r : r;
}
/* }}} */

/* {{{ rational_compare_exact
 *
 * Matches the PHP package's Rational::compareExact(): compares num/den exactly, without overflow, with
 * other_num/other_den, or, if other_float isn't NULL, with *other_float, which must be finite and strictly between
 * -2^63 and 2^63. Compares the signs, then the whole number parts of the magnitudes, then their fractions.
 */
static int rational_compare_exact(
	zend_long num, zend_long den, zend_long other_num, zend_long other_den, const double *other_float
)
{
	/* Compare the signs. */
	int sign = (num > 0) - (num < 0);
	int other_sign = other_float ? (*other_float > 0) - (*other_float < 0) : (other_num > 0) - (other_num < 0);
	if (sign != other_sign || sign == 0) {
		return sign < other_sign ? -1 : (sign > other_sign ? 1 : 0);
	}

	/* The signs are the same, so compare the magnitudes, and flip the result if they're negative. */
	zend_long whole, rem, other_whole, other_rem = 0;
	rational_split_magnitude(num, den, &whole, &rem);
	double other_frac = 0.0;
	if (other_float) {
		double abs_value = fabs(*other_float);
		other_whole = (zend_long) floor(abs_value);
		/* By Sterbenz's lemma, this subtraction is exact, as floor(abs_value) is either 0 or at least abs_value / 2. */
		other_frac = abs_value - floor(abs_value);
	} else {
		rational_split_magnitude(other_num, other_den, &other_whole, &other_rem);
	}

	int result = whole < other_whole ? -1 : (whole > other_whole ? 1 : 0);
	if (result == 0) {
		result = rational_compare_fractions(rem, den, other_rem, other_den, other_float ? &other_frac : NULL);
	}
	return sign * result;
}
/* }}} */

/* {{{ rational_compare_parts
 *
 * Three-way comparison of two already-canonical (numerator, denominator) pairs. Never fails
 * (always SUCCESS): a Rational is always finite by construction, so there's no NAN-style
 * "no meaningful result" case to report once both sides are already resolved to a pair of longs.
 * Also used by rational_calc_mod() (rational_arithmetic.c).
 */
void rational_compare_parts(zend_long a_num, zend_long a_den, zend_long b_num, zend_long b_den, int *out)
{
	/* If the denominators are equal, we only need to compare numerators. */
	if (a_den == b_den) {
		*out = a_num < b_num ? -1 : (a_num > b_num ? 1 : 0);
		return;
	}

	/* Avoid a float conversion by cross multiplying and comparing a*d with b*c (for a/b vs c/d). */
	zend_long ad, bc;
	if (math_integers_mul(a_num, b_den, &ad) == MATH_INTEGERS_OK
		&& math_integers_mul(a_den, b_num, &bc) == MATH_INTEGERS_OK) {
		*out = ad < bc ? -1 : (ad > bc ? 1 : 0);
		return;
	}

	/* In case of integer overflow, use the slower exact comparison. */
	*out = rational_compare_exact(a_num, a_den, b_num, b_den, NULL);
}
/* }}} */

/* {{{ rational_calc_compare
 *
 * The computational core of compare(), factored out so equal()/lessThan()/lessThanOrEqual()/
 * greaterThan()/greaterThanOrEqual()/approxCompare() can all call it directly, matching how the
 * PHP source's Comparable trait derives all of those from a single compare() method.
 *
 * Matches the PHP package's Rational::compare() exactly: an int operand is promoted to a
 * temporary (numerator, 1) pair directly (skipping actual object construction - a denominator of
 * 1 is always already canonical, so there's nothing rational_create() would do beyond copying the
 * values); a float operand compares exactly via rational_compare_exact(); a Rational operand
 * compares via rational_compare_parts(). PHP_INT_MIN is special-cased the same way the PHP source is: a
 * Rational can never equal PHP_INT_MIN (promoting it to a Rational would throw), but a Rational is
 * always greater than it.
 */
zend_result rational_calc_compare(zend_object *self_obj, zval *other, int *out)
{
	zend_long self_num, self_den;
	rational_read_parts(self_obj, &self_num, &self_den);

	if (Z_TYPE_P(other) == IS_LONG) {
		zend_long other_long = Z_LVAL_P(other);
		if (other_long == ZEND_LONG_MIN) {
			*out = 1;
			return SUCCESS;
		}
		rational_compare_parts(self_num, self_den, other_long, 1, out);
		return SUCCESS;
	}

	if (Z_TYPE_P(other) == IS_DOUBLE) {
		double other_dbl = Z_DVAL_P(other);
		if (zend_isnan(other_dbl)) {
			zend_throw_exception(spl_ce_DomainException, "Cannot compare Rational with NAN.", 0);
			return FAILURE;
		}
		/* Compare exactly, rather than converting this Rational to a float, which can round. Every Rational is
		 * strictly between -2^63 and 2^63, so a float outside that range decides the result. */
		double limit = -(double) ZEND_LONG_MIN;
		if (other_dbl >= limit) {
			*out = -1;
		} else if (other_dbl <= -limit) {
			*out = 1;
		} else if (self_num >= -MATH_MAX_EXACT_INT && self_num <= MATH_MAX_EXACT_INT && self_den <= MATH_MAX_EXACT_INT
			&& (double) self_num / (double) self_den != other_dbl) {
			/* Fast path: the numerator and denominator are exactly representable as doubles, so the quotient is
			 * correctly rounded. Rounding preserves order, so if it differs from other_dbl, it's on the same side. */
			double value = (double) self_num / (double) self_den;
			*out = value < other_dbl ? -1 : 1;
		} else {
			*out = rational_compare_exact(self_num, self_den, 0, 1, &other_dbl);
		}
		return SUCCESS;
	}

	if (Z_TYPE_P(other) == IS_OBJECT && instanceof_function(Z_OBJCE_P(other), rational_ce_Rational)) {
		zend_long other_num, other_den;
		rational_read_parts(Z_OBJ_P(other), &other_num, &other_den);
		rational_compare_parts(self_num, self_den, other_num, other_den, out);
		return SUCCESS;
	}

	zend_string *msg = strpprintf(
		0, "Cannot compare Rational with %s. Must be Rational, int, or float.", math_types_debug_type_name(other));
	zend_throw_exception(spl_ce_InvalidArgumentException, ZSTR_VAL(msg), 0);
	zend_string_release(msg);
	return FAILURE;
}
/* }}} */

/* {{{ rational_calc_approx_equal
 *
 * The computational core of approxEqual(), factored out so approxCompare() can call it directly.
 * Matches the PHP package's Rational::approxEqual(): converts both sides to float and delegates
 * to math_floats_approx_equal() (see floats.c).
 */
static zend_result rational_calc_approx_equal(zend_object *self_obj, zval *other, double rel_tol, double abs_tol, bool *out)
{
	double other_dbl;

	if (Z_TYPE_P(other) == IS_LONG) {
		other_dbl = (double) Z_LVAL_P(other);
	} else if (Z_TYPE_P(other) == IS_DOUBLE) {
		other_dbl = Z_DVAL_P(other);
		if (zend_isnan(other_dbl)) {
			zend_throw_exception(spl_ce_DomainException, "Cannot compare Rational with NAN.", 0);
			return FAILURE;
		}
	} else if (Z_TYPE_P(other) == IS_OBJECT && instanceof_function(Z_OBJCE_P(other), rational_ce_Rational)) {
		zend_long other_num, other_den;
		rational_read_parts(Z_OBJ_P(other), &other_num, &other_den);
		other_dbl = (double) other_num / (double) other_den;
	} else {
		zend_string *msg = strpprintf(
			0, "Cannot compare Rational with %s. Must be Rational, int, or float.", math_types_debug_type_name(other));
		zend_throw_exception(spl_ce_InvalidArgumentException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	zend_long self_num, self_den;
	rational_read_parts(self_obj, &self_num, &self_den);
	double self_float = (double) self_num / (double) self_den;

	if (math_floats_validate_tolerances(rel_tol, abs_tol) == FAILURE) {
		return FAILURE;
	}

	*out = math_floats_approx_equal(self_float, other_dbl, rel_tol, abs_tol);
	return SUCCESS;
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::compare(mixed $other): int */
PHP_METHOD(OceanMoon_Math_Rational, compare)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(other)
	ZEND_PARSE_PARAMETERS_END();

	int cmp;
	if (rational_calc_compare(Z_OBJ_P(ZEND_THIS), other, &cmp) == FAILURE) {
		RETURN_THROWS();
	}
	RETURN_LONG(cmp);
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::equal(mixed $other): bool */
PHP_METHOD(OceanMoon_Math_Rational, equal)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(other)
	ZEND_PARSE_PARAMETERS_END();

	int cmp;
	if (rational_calc_compare(Z_OBJ_P(ZEND_THIS), other, &cmp) == FAILURE) {
		RETURN_THROWS();
	}
	RETURN_BOOL(cmp == 0);
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::lessThan(mixed $other): bool */
PHP_METHOD(OceanMoon_Math_Rational, lessThan)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(other)
	ZEND_PARSE_PARAMETERS_END();

	int cmp;
	if (rational_calc_compare(Z_OBJ_P(ZEND_THIS), other, &cmp) == FAILURE) {
		RETURN_THROWS();
	}
	RETURN_BOOL(cmp == -1);
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::lessThanOrEqual(mixed $other): bool */
PHP_METHOD(OceanMoon_Math_Rational, lessThanOrEqual)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(other)
	ZEND_PARSE_PARAMETERS_END();

	int cmp;
	if (rational_calc_compare(Z_OBJ_P(ZEND_THIS), other, &cmp) == FAILURE) {
		RETURN_THROWS();
	}
	RETURN_BOOL(cmp != 1);
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::greaterThan(mixed $other): bool */
PHP_METHOD(OceanMoon_Math_Rational, greaterThan)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(other)
	ZEND_PARSE_PARAMETERS_END();

	int cmp;
	if (rational_calc_compare(Z_OBJ_P(ZEND_THIS), other, &cmp) == FAILURE) {
		RETURN_THROWS();
	}
	RETURN_BOOL(cmp == 1);
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::greaterThanOrEqual(mixed $other): bool */
PHP_METHOD(OceanMoon_Math_Rational, greaterThanOrEqual)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(other)
	ZEND_PARSE_PARAMETERS_END();

	int cmp;
	if (rational_calc_compare(Z_OBJ_P(ZEND_THIS), other, &cmp) == FAILURE) {
		RETURN_THROWS();
	}
	RETURN_BOOL(cmp != -1);
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::approxEqual(mixed $other, float $relTol = 1e-9, float $absTol = PHP_FLOAT_EPSILON): bool */
PHP_METHOD(OceanMoon_Math_Rational, approxEqual)
{
	zval *other;
	double rel_tol = 1e-9;
	double abs_tol = DBL_EPSILON;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_ZVAL(other)
		Z_PARAM_OPTIONAL
		Z_PARAM_DOUBLE(rel_tol)
		Z_PARAM_DOUBLE(abs_tol)
	ZEND_PARSE_PARAMETERS_END();

	bool result;
	if (rational_calc_approx_equal(Z_OBJ_P(ZEND_THIS), other, rel_tol, abs_tol, &result) == FAILURE) {
		RETURN_THROWS();
	}
	RETURN_BOOL(result);
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::approxCompare(mixed $other, float $relTol = 1e-9, float $absTol = PHP_FLOAT_EPSILON): int
 *
 * Matches ApproxComparable::approxCompare(): 0 if approxEqual() (within tolerance), otherwise
 * falls back to exact compare() for ordering.
 */
PHP_METHOD(OceanMoon_Math_Rational, approxCompare)
{
	zval *other;
	double rel_tol = 1e-9;
	double abs_tol = DBL_EPSILON;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_ZVAL(other)
		Z_PARAM_OPTIONAL
		Z_PARAM_DOUBLE(rel_tol)
		Z_PARAM_DOUBLE(abs_tol)
	ZEND_PARSE_PARAMETERS_END();

	bool approx_eq;
	if (rational_calc_approx_equal(Z_OBJ_P(ZEND_THIS), other, rel_tol, abs_tol, &approx_eq) == FAILURE) {
		RETURN_THROWS();
	}
	if (approx_eq) {
		RETURN_LONG(0);
	}

	int cmp;
	if (rational_calc_compare(Z_OBJ_P(ZEND_THIS), other, &cmp) == FAILURE) {
		RETURN_THROWS();
	}
	RETURN_LONG(cmp);
}
/* }}} */
