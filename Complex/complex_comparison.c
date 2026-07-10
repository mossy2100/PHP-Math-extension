/*
 * complex_comparison.c
 *
 * Comparison methods for OceanMoon\Math\Complex: equal() and approxEqual(). Mirrors the
 * "Comparison methods" region of the PHP package's Complex class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <float.h>

#include "php.h"
#include "php_math.h"
#include "complex_internal.h"
#include "complex_arginfo.h"
#include "floats.h"

/* {{{ complex_normalize_operand
 *
 * Shared by equal() and approxEqual(): resolves $other into a (real, imaginary) pair the way
 * the PHP package's Complex::equal()/approxEqual() do -- an int or float is treated as a real
 * Complex (zero imaginary part); an existing Complex instance contributes its own parts. Returns
 * false (nothing else set) for any other type, which both callers treat as "not equal".
 */
static bool complex_normalize_operand(zval *value, double *out_real, double *out_imag)
{
	if (Z_TYPE_P(value) == IS_LONG || Z_TYPE_P(value) == IS_DOUBLE) {
		*out_real = zval_get_double(value);
		*out_imag = 0.0;
		return true;
	}

	if (Z_TYPE_P(value) == IS_OBJECT && instanceof_function(Z_OBJCE_P(value), complex_ce_Complex)) {
		zval rv1, rv2;
		zend_object *obj = Z_OBJ_P(value);
		*out_real = zval_get_double(
			zend_read_property(complex_ce_Complex, obj, "real", sizeof("real") - 1, 1, &rv1));
		*out_imag = zval_get_double(
			zend_read_property(complex_ce_Complex, obj, "imaginary", sizeof("imaginary") - 1, 1, &rv2));
		return true;
	}

	return false;
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::equal(mixed $other): bool
 *
 * Matches the PHP package's Complex::equal(): exact equality of real/imaginary parts.
 */
PHP_METHOD(OceanMoon_Math_Complex, equal)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(other)
	ZEND_PARSE_PARAMETERS_END();

	double other_real, other_imag;
	if (!complex_normalize_operand(other, &other_real, &other_imag)) {
		RETURN_FALSE;
	}

	zval rv1, rv2;
	zend_object *self = Z_OBJ_P(ZEND_THIS);
	double real = zval_get_double(
		zend_read_property(complex_ce_Complex, self, "real", sizeof("real") - 1, 1, &rv1));
	double imag = zval_get_double(
		zend_read_property(complex_ce_Complex, self, "imaginary", sizeof("imaginary") - 1, 1, &rv2));

	RETURN_BOOL(real == other_real && imag == other_imag);
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::approxEqual(mixed $other, float $relTol = 1e-9, float $absTol = PHP_FLOAT_EPSILON): bool
 *
 * Matches the PHP package's Complex::approxEqual(): both real and imaginary parts must be
 * approximately equal, via math_floats_approx_equal() (see floats.c).
 */
PHP_METHOD(OceanMoon_Math_Complex, approxEqual)
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

	double other_real, other_imag;
	if (!complex_normalize_operand(other, &other_real, &other_imag)) {
		RETURN_FALSE;
	}

	zval rv1, rv2;
	zend_object *self = Z_OBJ_P(ZEND_THIS);
	double real = zval_get_double(
		zend_read_property(complex_ce_Complex, self, "real", sizeof("real") - 1, 1, &rv1));
	double imag = zval_get_double(
		zend_read_property(complex_ce_Complex, self, "imaginary", sizeof("imaginary") - 1, 1, &rv2));

	RETURN_BOOL(
		math_floats_approx_equal(real, other_real, rel_tol, abs_tol) &&
		math_floats_approx_equal(imag, other_imag, rel_tol, abs_tol)
	);
}
/* }}} */
