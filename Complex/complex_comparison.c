/*
 * complex_comparison.c
 *
 * Comparison methods for OceanMoon\Math\Complex: equal() and approxEqual(). Mirrors the "Comparison
 * methods" region of the PHP package's Complex class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <float.h>

#include "php.h"
#include "php_math.h"
#include "Zend/zend_exceptions.h"
#include "complex_internal.h"
#include "complex_arginfo.h"
#include "floats.h"

/* {{{ complex_read_parts
 *
 * Shared by equal(), approxEqual(), and complex_normalize_operand(): reads the real
 * and imaginary parts off a zend_object already known to be a Complex instance.
 */
static void complex_read_parts(zend_object *obj, double *out_real, double *out_imag)
{
	zval rv1, rv2;
	*out_real = zval_get_double(
		zend_read_property(complex_ce_Complex, obj, "real", sizeof("real") - 1, 1, &rv1));
	*out_imag = zval_get_double(
		zend_read_property(complex_ce_Complex, obj, "imaginary", sizeof("imaginary") - 1, 1, &rv2));
}
/* }}} */

/* {{{ complex_normalize_operand
 *
 * Shared by equal() and approxEqual(): resolves $other into a (real, imaginary) pair by
 * delegating to complex_to_complex(), exactly mirroring how the PHP package's equal()/
 * approxEqual() call self::toComplex($other) and swallow any resulting exception. Returns false
 * (nothing else set, and any pending exception cleared) if $other can't be converted, which both
 * callers treat as "not equal" rather than propagating the exception.
 */
static bool complex_normalize_operand(zval *value, double *out_real, double *out_imag)
{
	zval converted;
	if (complex_to_complex(&converted, value) == FAILURE) {
		if (EG(exception)) {
			zend_clear_exception();
		}
		return false;
	}

	complex_read_parts(Z_OBJ(converted), out_real, out_imag);

	zval_ptr_dtor(&converted);
	return true;
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

	double real, imag;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &real, &imag);

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

	double real, imag;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &real, &imag);

	RETURN_BOOL(
		math_floats_approx_equal(real, other_real, rel_tol, abs_tol) &&
		math_floats_approx_equal(imag, other_imag, rel_tol, abs_tol)
	);
}
/* }}} */
