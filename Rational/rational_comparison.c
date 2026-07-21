/*
 * rational_comparison.c
 *
 * Comparison methods for OceanMoon\Math\Rational: compare(), approxEqual(), plus the methods the
 * PHP package's Rational class inherits from the ApproxComparable trait (equal(), lessThan(),
 * lessThanOrEqual(), greaterThan(), greaterThanOrEqual(), approxCompare()) -- internal classes
 * can't use traits, so this file ports both the class's own methods and the trait's, matching the
 * "Comparison methods" region of the PHP package's Rational class plus
 * OceanMoon\Core\Traits\Comparison\Comparable/ApproxComparable.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <float.h>

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "rational_internal.h"
#include "rational_arginfo.h"
#include "floats.h"
#include "integers.h"
#include "types.h"

/* {{{ rational_compare_parts
 *
 * Three-way comparison of two already-canonical (numerator, denominator) pairs. Never fails
 * (always SUCCESS): a Rational is always finite by construction, so there's no NAN-style
 * "no meaningful result" case to report once both sides are already resolved to a pair of longs.
 */
static void rational_compare_parts(zend_long a_num, zend_long a_den, zend_long b_num, zend_long b_den, int *out)
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

	/* In case of integer overflow, compare equivalent floating point values. This could produce a
	 * result of 0 (equal) if two different rationals convert to the same float, which is possible
	 * for values with a magnitude >= 2^53, but that's ok (matches the PHP source's own caveat). */
	double a_float = (double) a_num / (double) a_den;
	double b_float = (double) b_num / (double) b_den;
	*out = a_float < b_float ? -1 : (a_float > b_float ? 1 : 0);
}
/* }}} */

/* {{{ rational_calc_compare
 *
 * The computational core of compare(), factored out so equal()/lessThan()/lessThanOrEqual()/
 * greaterThan()/greaterThanOrEqual()/approxCompare() can all call it directly, matching how the
 * PHP source's Comparable trait derives all of those from a single compare() method.
 *
 * Matches the PHP package's Rational::compare() exactly: an int operand is promoted to a
 * temporary (numerator, 1) pair directly (skipping actual object construction -- a denominator of
 * 1 is always already canonical, so there's nothing rational_create() would do beyond copying the
 * values); a float operand compares via toFloat(); a Rational operand compares via
 * rational_compare_parts(). PHP_INT_MIN is special-cased the same way the PHP source is: a
 * Rational can never equal PHP_INT_MIN (promoting it to a Rational would throw), but a Rational is
 * always greater than it.
 */
static zend_result rational_calc_compare(zend_object *self_obj, zval *other, int *out)
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
		double self_float = (double) self_num / (double) self_den;
		*out = self_float < other_dbl ? -1 : (self_float > other_dbl ? 1 : 0);
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
