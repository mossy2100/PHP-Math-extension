/*
 * matrix_linear_algebra.c
 *
 * Linear algebra methods for OceanMoon\Math\Matrix: t(), det(), trace(). Mirrors the "Linear
 * algebra methods" region of the PHP package's Matrix class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "matrix_internal.h"
#include "matrix_arginfo.h"

/* {{{ matrix_calc_mul_vector
 *
 * Computes "matrix times vector" (Ax): row i of the result is the dot product of this Matrix's
 * row i with $vector. Matches the PHP package's `$this->mul($vector->toColumnMatrix())->
 * getColumn(0)`, computed directly here (rather than building an intermediate column Matrix) for
 * efficiency. Used only by the `*` operator's `Matrix * Vector` form (matrix_operators.c) --
 * Matrix has no mulVector() method of its own (see Matrix::mul()'s doc comment in the PHP
 * package for why).
 */
zend_result matrix_calc_mul_vector(zend_object *self, zend_object *vector, zval *return_value)
{
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);
	zend_long vector_size = vector_read_size(vector);

	if (column_count != vector_size) {
		zend_string *msg = strpprintf(
			0, "Invalid Matrix row count: " ZEND_LONG_FMT ". Must equal this Matrix's column count: "
			ZEND_LONG_FMT ".", vector_size, column_count
		);
		zend_throw_exception(spl_ce_LengthException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	if (vector_create(return_value, row_count) == FAILURE) {
		return FAILURE;
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < row_count; i++) {
		double sum = 0.0;
		for (zend_long k = 0; k < column_count; k++) {
			double a, b;
			matrix_read_element(self, i, k, &a);
			vector_read_element(vector, k, &b);
			sum += a * b;
		}
		if (vector_write_element(result, i, sum) == FAILURE) {
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			return FAILURE;
		}
	}

	return SUCCESS;
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::t(): Matrix
 *
 * Matches the PHP package's Matrix::t(): the transpose.
 */
PHP_METHOD(OceanMoon_Math_Matrix, t)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);

	if (matrix_create(return_value, column_count, row_count) == FAILURE) {
		RETURN_THROWS();
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < row_count; i++) {
		for (zend_long j = 0; j < column_count; j++) {
			double value;
			matrix_read_element(self, i, j, &value);
			if (matrix_write_element(result, j, i, value) == FAILURE) {
				zval_ptr_dtor(return_value);
				ZVAL_UNDEF(return_value);
				RETURN_THROWS();
			}
		}
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::det(): float
 *
 * Matches the PHP package's Matrix::det(): `$this->calcDet($this->toArray())`.
 */
PHP_METHOD(OceanMoon_Math_Matrix, det)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);

	if (row_count != column_count) {
		zend_throw_exception(spl_ce_DomainException, "Cannot compute determinant of non-square Matrix.", 0);
		RETURN_THROWS();
	}

	zend_long size = row_count;
	zval data_array;
	array_init_size(&data_array, (uint32_t) size);
	for (zend_long i = 0; i < size; i++) {
		zval row;
		array_init_size(&row, (uint32_t) size);
		for (zend_long j = 0; j < size; j++) {
			double value;
			matrix_read_element(self, i, j, &value);
			zval v;
			ZVAL_DOUBLE(&v, value);
			zend_hash_next_index_insert(Z_ARRVAL(row), &v);
		}
		zend_hash_next_index_insert(Z_ARRVAL(data_array), &row);
	}

	double det = matrix_calc_det(&data_array);
	zval_ptr_dtor(&data_array);

	RETURN_DOUBLE(det);
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::trace(): float
 *
 * Matches the PHP package's Matrix::trace(): sum of diagonal elements.
 */
PHP_METHOD(OceanMoon_Math_Matrix, trace)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);

	if (row_count != column_count) {
		zend_throw_exception(spl_ce_DomainException, "Cannot compute trace of non-square Matrix.", 0);
		RETURN_THROWS();
	}

	double sum = 0.0;
	for (zend_long i = 0; i < row_count; i++) {
		double value;
		matrix_read_element(self, i, i, &value);
		sum += value;
	}

	RETURN_DOUBLE(sum);
}
/* }}} */
