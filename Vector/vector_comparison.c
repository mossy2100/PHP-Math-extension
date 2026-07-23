/*
 * vector_comparison.c
 *
 * Comparison methods for OceanMoon\Math\Vector: equal(), approxEqual(). Mirrors the "Comparison
 * methods" region of the PHP package's Vector class -- both only accept another Vector (same-type
 * comparison only, per the finalized comparison-trait policy: mixed $other, never self, throw for
 * anything else).
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "vector_internal.h"
#include "vector_arginfo.h"
#include "floats.h"
#include "types.h"
#include <float.h>

/* {{{ vector_check_same_size_operand
 *
 * Shared by equal() and approxEqual(): checks $other is a Vector, throwing InvalidArgumentException
 * otherwise. Matches the PHP package's own type check; the size comparison (returning false, not
 * throwing, for a size mismatch) is handled separately by each caller.
 */
static zend_result vector_check_operand(zval *other)
{
	if (Z_TYPE_P(other) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(other), vector_ce_Vector)) {
		zend_string *msg = strpprintf(
			0, "Cannot compare Vector with %s. Must be Vector.", math_types_debug_type_name(other)
		);
		zend_throw_exception(spl_ce_InvalidArgumentException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::equal(mixed $other): bool
 *
 * Matches the PHP package's Vector::equal(): same size and every element exactly equal.
 */
PHP_METHOD(OceanMoon_Math_Vector, equal)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(other)
	ZEND_PARSE_PARAMETERS_END();

	if (vector_check_operand(other) == FAILURE) {
		RETURN_THROWS();
	}

	zend_object *self_obj = Z_OBJ_P(ZEND_THIS);
	zend_object *other_obj = Z_OBJ_P(other);

	zend_long size = vector_read_size(self_obj);
	if (size != vector_read_size(other_obj)) {
		RETURN_FALSE;
	}

	for (zend_long i = 0; i < size; i++) {
		double a, b;
		vector_read_element(self_obj, i, &a);
		vector_read_element(other_obj, i, &b);
		if (a != b) {
			RETURN_FALSE;
		}
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::approxEqual(mixed $other, float $relTol = 1e-9, float $absTol = PHP_FLOAT_EPSILON): bool
 *
 * Matches the PHP package's Vector::approxEqual(): same size and every element approximately
 * equal, via math_floats_approx_equal().
 */
PHP_METHOD(OceanMoon_Math_Vector, approxEqual)
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

	if (vector_check_operand(other) == FAILURE) {
		RETURN_THROWS();
	}

	zend_object *self_obj = Z_OBJ_P(ZEND_THIS);
	zend_object *other_obj = Z_OBJ_P(other);

	zend_long size = vector_read_size(self_obj);
	if (size != vector_read_size(other_obj)) {
		RETURN_FALSE;
	}

	for (zend_long i = 0; i < size; i++) {
		double a, b;
		vector_read_element(self_obj, i, &a);
		vector_read_element(other_obj, i, &b);
		if (!math_floats_approx_equal(a, b, rel_tol, abs_tol)) {
			RETURN_FALSE;
		}
	}

	RETURN_TRUE;
}
/* }}} */
