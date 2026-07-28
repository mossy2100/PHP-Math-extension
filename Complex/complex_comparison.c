/*
 * complex_comparison.c
 *
 * Comparison methods for OceanMoon\Math\Complex: equal() and approxEqual() (mirroring the
 * "Comparison methods" region of the PHP package's Complex class, which has no natural ordering
 * of its own), plus complex_calc_compare()/complex_do_compare(), the extension-only <, <=, >, >=,
 * <=> operators (and, as an unavoidable side effect of PHP wiring all of these to the same
 * handler, ==/!=). These don't exist in the PHP package: comparing complex numbers this way isn't
 * mathematically meaningful (there's no total order compatible with the field operations), but a
 * deterministic total order is still useful for sorting/deduplication, so the operators use plain
 * lexicographic (real, then imaginary) ordering -- exactly what PHP's default object comparison
 * already gives you for two Complex instances for free (real is declared before imaginary), which
 * is why the PHP package needs no changes at all for this. The only thing that genuinely requires
 * new code is accepting an int|float operand on either side (promoted the same way equal() does).
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

/* {{{ complex_normalize_operand
 *
 * Shared by equal(), approxEqual(), and complex_calc_compare(): resolves $other into a (real,
 * imaginary) pair, matching the PHP package's shared type-check logic. $other must be a Complex
 * instance or a number (int/float); anything else throws InvalidArgumentException. A NAN float
 * throws DomainException (no meaningful comparison result); a non-finite float (+-INF) still
 * populates *out_real/*out_imag (with the signed infinity itself, imaginary 0) but also reports
 * *out_finite = false, since a Complex is always finite and so never equal to it -- equal()/
 * approxEqual() use that to return false outright, while complex_calc_compare() uses the sign of
 * *out_real to order against it instead.
 *
 * Returns FAILURE (with an exception already thrown) for a type mismatch or NAN.
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
			*out_real = d;
			*out_imag = 0.0;
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

/* {{{ complex_calc_compare
 *
 * The computational core of the <, <=, >, >=, <=> operators (complex_do_compare(), below): plain
 * lexicographic (real, then imaginary) ordering, matching PHP's own default object comparison for
 * two Complex instances (real is declared before imaginary) -- the only genuinely new behavior
 * here is accepting an int|float operand via complex_normalize_operand(), the same promotion
 * equal()/approxEqual() use.
 *
 * The +-INF case (complex_normalize_operand() reports *finite = false) has no equal() equivalent
 * to fall back on, since ordering (unlike equality) needs a direction: a Complex is always finite,
 * so it's always less than +INF and always greater than -INF, which the sign of the now-populated
 * other_real (see complex_normalize_operand()'s doc comment) gives directly.
 *
 * Returns FAILURE (with an exception already thrown) for a type mismatch or NAN, matching
 * complex_normalize_operand(). On SUCCESS, *out is -1, 0, or 1.
 */
zend_result complex_calc_compare(zend_object *self_obj, zval *other, int *out)
{
	double other_real, other_imag;
	bool finite;
	if (complex_normalize_operand(other, &other_real, &other_imag, &finite) == FAILURE) {
		return FAILURE;
	}

	double real, imag;
	complex_read_parts(self_obj, &real, &imag);

	if (!finite) {
		*out = other_real > 0 ? -1 : 1;
		return SUCCESS;
	}

	if (real != other_real) {
		*out = real < other_real ? -1 : 1;
		return SUCCESS;
	}

	*out = imag < other_imag ? -1 : (imag > other_imag ? 1 : 0);
	return SUCCESS;
}
/* }}} */

/* {{{ complex_do_compare
 *
 * The `compare` object handler backing Complex's comparison operators: <, <=, >, >=, <=>, and (as
 * an unavoidable side effect of PHP wiring all of these to the same handler) ==/!=. Never affects
 * ===/!==, which always mean reference identity for objects regardless of this handler (see
 * zend_is_identical() in Zend/zend_operators.c).
 *
 * Called with the original op1/op2 in their expression order, whichever side the Complex instance
 * is actually on (PHP picks op1's handler if op1 is a suitable object, else op2's -- see
 * zend_compare()'s IS_OBJECT branch in Zend/zend_operators.c) -- so this determines which side is
 * `self` and, if it's op2 (e.g. `5 <=> $z`), negates the result to correct for comparing in the
 * opposite direction from what complex_calc_compare() (self vs. other) computes.
 *
 * Delegates entirely to complex_calc_compare() above. If it fails, an exception is already
 * pending; the returned int value is irrelevant once that's true (the engine checks for a pending
 * exception after calling this handler and aborts before using the result).
 */
int complex_do_compare(zval *op1, zval *op2)
{
	zval *self_zv, *other;
	bool self_is_op1;

	if (Z_TYPE_P(op1) == IS_OBJECT && instanceof_function(Z_OBJCE_P(op1), complex_ce_Complex)) {
		self_zv = op1;
		other = op2;
		self_is_op1 = true;
	} else {
		self_zv = op2;
		other = op1;
		self_is_op1 = false;
	}

	int cmp;
	if (complex_calc_compare(Z_OBJ_P(self_zv), other, &cmp) == FAILURE) {
		return 0;
	}

	return self_is_op1 ? cmp : -cmp;
}
/* }}} */
