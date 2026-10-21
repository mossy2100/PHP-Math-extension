/*
 * matrix_comparison.c
 *
 * Comparison methods for OceanMoon\Math\Matrix: equal(), approxEqual(). Mirrors the "Comparison
 * methods" region of the PHP package's Matrix class - both only accept another Matrix (same-type
 * comparison only, per the finalized comparison-trait policy: mixed $other, never self, throw for
 * anything else).
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <float.h>

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "matrix_internal.h"
#include "../../oceanmoon_math_arginfo.h"
#include "floats.h"
#include "types.h"

/* {{{ matrix_check_operand
 *
 * Shared by equal() and approxEqual(): checks $other is a Matrix, throwing InvalidArgumentException
 * otherwise.
 */
static zend_result matrix_check_operand(zval *other)
{
	if (Z_TYPE_P(other) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(other), matrix_ce_Matrix)) {
		zend_string *msg = strpprintf(
			0, "Cannot compare Matrix with %s. Must be Matrix.", math_types_debug_type_name(other)
		);
		zend_throw_exception(spl_ce_InvalidArgumentException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::equal(mixed $other): bool
 *
 * Matches the PHP package's Matrix::equal(): same dimensions and every element exactly equal.
 */
PHP_METHOD(OceanMoon_Math_Matrix, equal)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(other)
	ZEND_PARSE_PARAMETERS_END();

	if (matrix_check_operand(other) == FAILURE) {
		RETURN_THROWS();
	}

	zend_object *self_obj = Z_OBJ_P(ZEND_THIS);
	zend_object *other_obj = Z_OBJ_P(other);

	zend_long row_count = matrix_read_row_count(self_obj);
	zend_long column_count = matrix_read_column_count(self_obj);
	if (row_count != matrix_read_row_count(other_obj) || column_count != matrix_read_column_count(other_obj)) {
		RETURN_FALSE;
	}

	double *values = matrix_read_all(self_obj);
	if (values == NULL) {
		RETURN_THROWS();
	}
	double *other_values = matrix_read_all(other_obj);
	if (other_values == NULL) {
		efree(values);
		RETURN_THROWS();
	}

	bool equal = true;
	zend_long count = row_count * column_count;
	for (zend_long k = 0; k < count && equal; k++) {
		equal = values[k] == other_values[k];
	}
	efree(values);
	efree(other_values);

	RETURN_BOOL(equal);
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::approxEqual(mixed $other, float $relTol = 1e-9, float $absTol = PHP_FLOAT_EPSILON): bool
 *
 * Matches the PHP package's Matrix::approxEqual().
 */
PHP_METHOD(OceanMoon_Math_Matrix, approxEqual)
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

	if (matrix_check_operand(other) == FAILURE) {
		RETURN_THROWS();
	}

	zend_object *self_obj = Z_OBJ_P(ZEND_THIS);
	zend_object *other_obj = Z_OBJ_P(other);

	zend_long row_count = matrix_read_row_count(self_obj);
	zend_long column_count = matrix_read_column_count(self_obj);
	if (row_count != matrix_read_row_count(other_obj) || column_count != matrix_read_column_count(other_obj)) {
		RETURN_FALSE;
	}

	/* Like the package, the tolerances are only checked once there's an element to compare. */
	if (row_count > 0 && column_count > 0 && math_floats_validate_tolerances(rel_tol, abs_tol) == FAILURE) {
		RETURN_THROWS();
	}

	double *values = matrix_read_all(self_obj);
	if (values == NULL) {
		RETURN_THROWS();
	}
	double *other_values = matrix_read_all(other_obj);
	if (other_values == NULL) {
		efree(values);
		RETURN_THROWS();
	}

	bool equal = true;
	zend_long count = row_count * column_count;
	for (zend_long k = 0; k < count && equal; k++) {
		equal = math_floats_approx_equal(values[k], other_values[k], rel_tol, abs_tol);
	}
	efree(values);
	efree(other_values);

	RETURN_BOOL(equal);
}
/* }}} */
