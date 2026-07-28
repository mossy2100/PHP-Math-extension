/*
 * vector_linear_algebra.c
 *
 * Linear algebra methods for OceanMoon\Math\Vector: dot(), cross(), outer(), normalized(). Mirrors
 * the "Linear algebra methods" region of the PHP package's Vector class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <math.h>

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "vector_internal.h"
#include "vector_arginfo.h"
#include "../Matrix/matrix_internal.h"

/* {{{ vector_compute_magnitude
 *
 * Matches the PHP package's Vector::$magnitude hook: sqrt(sum of squares). Backs the `magnitude`
 * computed property (vector_properties.c), and is also called directly (bypassing property read
 * dispatch) by normalized() below and normalize() (vector_modification.c).
 */
double vector_compute_magnitude(zend_object *obj)
{
	zend_long count = vector_read_count(obj);
	double sum = 0.0;

	for (zend_long i = 0; i < count; i++) {
		double element;
		/* Bounds are trivially satisfied (0..count-1), so this never fails. */
		vector_read_element(obj, i, &element);
		sum += element * element;
	}

	return sqrt(sum);
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::dot(Vector $other): float
 *
 * Matches the PHP package's Vector::dot().
 */
PHP_METHOD(OceanMoon_Math_Vector, dot)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(other, vector_ce_Vector)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_object *other_obj = Z_OBJ_P(other);

	zend_long count = vector_read_count(self);
	zend_long other_count = vector_read_count(other_obj);
	if (count != other_count) {
		zend_string *msg = strpprintf(
			0, "Cannot compute dot product with Vector of incorrect count: " ZEND_LONG_FMT ". Expected "
			ZEND_LONG_FMT ".", other_count, count
		);
		zend_throw_exception(spl_ce_LengthException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		RETURN_THROWS();
	}

	double sum = 0.0;
	for (zend_long i = 0; i < count; i++) {
		double a, b;
		vector_read_element(self, i, &a);
		vector_read_element(other_obj, i, &b);
		sum += a * b;
	}

	RETURN_DOUBLE(sum);
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::cross(Vector $other): Vector
 *
 * Matches the PHP package's Vector::cross(): both vectors must be count 3.
 */
PHP_METHOD(OceanMoon_Math_Vector, cross)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(other, vector_ce_Vector)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_object *other_obj = Z_OBJ_P(other);

	zend_long self_count = vector_read_count(self);
	if (self_count != 3) {
		zend_string *msg = strpprintf(
			0, "Cannot compute cross product with first Vector count: " ZEND_LONG_FMT ". Must be 3.", self_count
		);
		zend_throw_exception(spl_ce_LengthException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		RETURN_THROWS();
	}

	zend_long other_count = vector_read_count(other_obj);
	if (other_count != 3) {
		zend_string *msg = strpprintf(
			0, "Cannot compute cross product with second Vector count: " ZEND_LONG_FMT ". Must be 3.", other_count
		);
		zend_throw_exception(spl_ce_LengthException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		RETURN_THROWS();
	}

	double a0, a1, a2, b0, b1, b2;
	vector_read_element(self, 0, &a0);
	vector_read_element(self, 1, &a1);
	vector_read_element(self, 2, &a2);
	vector_read_element(other_obj, 0, &b0);
	vector_read_element(other_obj, 1, &b1);
	vector_read_element(other_obj, 2, &b2);

	if (vector_create(return_value, 3) == FAILURE) {
		RETURN_THROWS();
	}
	zend_object *result = Z_OBJ_P(return_value);

	if (vector_write_element(result, 0, a1 * b2 - a2 * b1) == FAILURE
		|| vector_write_element(result, 1, a2 * b0 - a0 * b2) == FAILURE
		|| vector_write_element(result, 2, a0 * b1 - a1 * b0) == FAILURE) {
		zval_ptr_dtor(return_value);
		ZVAL_UNDEF(return_value);
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::outer(Vector $other): Matrix
 *
 * Matches the PHP package's Vector::outer(): `$this->toColumnMatrix()->mul($other->toRowMatrix())`.
 */
PHP_METHOD(OceanMoon_Math_Vector, outer)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(other, vector_ce_Vector)
	ZEND_PARSE_PARAMETERS_END();

	zval column_matrix;
	if (vector_to_column_matrix(Z_OBJ_P(ZEND_THIS), &column_matrix) == FAILURE) {
		RETURN_THROWS();
	}

	zval row_matrix;
	if (vector_to_row_matrix(Z_OBJ_P(other), &row_matrix) == FAILURE) {
		zval_ptr_dtor(&column_matrix);
		RETURN_THROWS();
	}

	zend_result result = matrix_calc_mul_matrix(Z_OBJ(column_matrix), Z_OBJ(row_matrix), return_value);
	zval_ptr_dtor(&column_matrix);
	zval_ptr_dtor(&row_matrix);
	if (result == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::normalized(): Vector
 *
 * Matches the PHP package's Vector::normalized(): div($this->magnitude), via the same
 * vector_calc_div() core div() itself uses (vector_arithmetic.c).
 */
PHP_METHOD(OceanMoon_Math_Vector, normalized)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	double magnitude = vector_compute_magnitude(self);

	if (vector_calc_div(self, magnitude, return_value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */
