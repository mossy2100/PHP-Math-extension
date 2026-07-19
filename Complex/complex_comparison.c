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
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "complex_internal.h"
#include "complex_arginfo.h"
#include "floats.h"
#include "types.h"

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
 * Shared by equal() and approxEqual(): resolves $other into a (real, imaginary) pair, matching
 * the PHP package's shared type-check logic. $other must be a Complex instance or a number
 * (int/float); anything else throws InvalidArgumentException. A NAN float throws DomainException
 * (no meaningful comparison result); a non-finite float (+-INF) returns false via *out_finite,
 * since a Complex is always finite and so never (approximately) equal to it.
 *
 * Returns FAILURE (with an exception already thrown) for a type mismatch or NAN. On SUCCESS,
 * *out_finite is false only for the +-INF case -- callers should return false without comparing
 * real/imaginary in that case.
 */
static zend_result complex_normalize_operand(zval *value, double *out_real, double *out_imag, bool *out_finite)
{
	*out_finite = true;

	if (Z_TYPE_P(value) == IS_OBJECT && instanceof_function(Z_OBJCE_P(value), complex_ce_Complex)) {
		complex_read_parts(Z_OBJ_P(value), out_real, out_imag);
		return SUCCESS;
	}

	if (Z_TYPE_P(value) != IS_LONG && Z_TYPE_P(value) != IS_DOUBLE) {
		zend_string *msg = strpprintf(0, "Cannot compare Complex with %s. Must be Complex, int, or float.",
			math_types_debug_type_name(value));
		zend_throw_exception(spl_ce_InvalidArgumentException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	if (Z_TYPE_P(value) == IS_DOUBLE) {
		double d = Z_DVAL_P(value);
		if (zend_isnan(d)) {
			zend_throw_exception(spl_ce_DomainException, "Cannot compare Complex with NAN.", 0);
			return FAILURE;
		}
		if (!zend_finite(d)) {
			*out_finite = false;
			return SUCCESS;
		}
	}

	*out_real = zval_get_double(value);
	*out_imag = 0.0;
	return SUCCESS;
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
	bool finite;
	if (complex_normalize_operand(other, &other_real, &other_imag, &finite) == FAILURE) {
		RETURN_THROWS();
	}
	if (!finite) {
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
	bool finite;
	if (complex_normalize_operand(other, &other_real, &other_imag, &finite) == FAILURE) {
		RETURN_THROWS();
	}
	if (!finite) {
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
