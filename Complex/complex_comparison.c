/*
 * complex_comparison.c
 *
 * Comparison methods for OceanMoon\Math\Complex: identical(), equal(), and approxEqual(). Also
 * the compare object handler backing ==/!= (see complex_compare_objects()). Mirrors the
 * "Comparison methods" region of the PHP package's Complex class.
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
 * Shared by identical(), equal(), approxEqual(), and complex_normalize_operand(): reads the real
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

/* {{{ OceanMoon\Math\Complex::identical(mixed $other): bool
 *
 * Matches the PHP package's Complex::identical(): true only if $other is actually a Complex
 * instance (not merely something toComplex() could convert) with exactly equal real/imaginary
 * parts. Since Complex is final, checking instanceof here is equivalent to the package's
 * Types::same() (get_debug_type() comparison) -- no subclass can exist to make them differ.
 */
PHP_METHOD(OceanMoon_Math_Complex, identical)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(other)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(other) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(other), complex_ce_Complex)) {
		RETURN_FALSE;
	}

	double real, imag, other_real, other_imag;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &real, &imag);
	complex_read_parts(Z_OBJ_P(other), &other_real, &other_imag);

	RETURN_BOOL(real == other_real && imag == other_imag);
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

/* {{{ complex_compare_objects
 *
 * The `compare` object handler backing ==/!=/<=> (and, degenerately, </>/<=/>=, for which Complex
 * has no meaningful ordering). Installed onto complex_object_handlers in complex_minit().
 *
 * A thin wrapper around the same logic as equal(): both operands are normalized via
 * complex_normalize_operand() (which already handles a Complex instance on either side, since
 * that's exactly what complex_to_complex()'s first branch does), then compared for exact
 * equality. Returns 0 (equal) or 1 (not equal, or either side isn't convertible) -- matching
 * zend_object_compare_t's strcmp-style contract, but only the zero/non-zero distinction is
 * meaningful here, since Complex numbers have no natural ordering.
 */
int complex_compare_objects(zval *op1, zval *op2)
{
	double real1, imag1, real2, imag2;

	if (!complex_normalize_operand(op1, &real1, &imag1) || !complex_normalize_operand(op2, &real2, &imag2)) {
		return 1;
	}

	return (real1 == real2 && imag1 == imag2) ? 0 : 1;
}
/* }}} */
