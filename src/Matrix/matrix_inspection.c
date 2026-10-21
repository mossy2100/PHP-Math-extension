/*
 * matrix_inspection.c
 *
 * Inspection methods for OceanMoon\Math\Matrix: isSquare(), get(), getRow(), getColumn(), copy().
 * Mirrors the "Inspection methods" region of the PHP package's Matrix class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "matrix_internal.h"
#include "../../oceanmoon_math_arginfo.h"

/* {{{ OceanMoon\Math\Matrix::isSquare(?int $size = null): bool
 *
 * Matches the PHP package's Matrix::isSquare().
 */
PHP_METHOD(OceanMoon_Math_Matrix, isSquare)
{
	zend_long size = 0;
	bool size_is_null = 1;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(size, size_is_null)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);

	RETURN_BOOL(row_count == column_count && (size_is_null || row_count == size));
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::get(int $row, int $col): float
 *
 * Matches the PHP package's Matrix::get().
 */
PHP_METHOD(OceanMoon_Math_Matrix, get)
{
	zend_long row, col;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(row)
		Z_PARAM_LONG(col)
	ZEND_PARSE_PARAMETERS_END();

	double value;
	if (matrix_read_element(Z_OBJ_P(ZEND_THIS), row, col, &value) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_DOUBLE(value);
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::getRow(int $row): Vector
 *
 * Matches the PHP package's Matrix::getRow(): `clone $this->data[$row]` - an independent copy,
 * unlike offsetGet()'s live reference.
 */
PHP_METHOD(OceanMoon_Math_Matrix, getRow)
{
	zend_long row;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(row)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long row_count = matrix_read_row_count(self);
	if (row < 0 || row >= row_count) {
		matrix_throw_index_error("row", row, row_count);
		RETURN_THROWS();
	}

	zend_object *row_obj = matrix_read_row(self, row);
	if (row_obj == NULL) {
		RETURN_THROWS();
	}
	zend_object *cloned = row_obj->handlers->clone_obj(row_obj);
	RETVAL_OBJ(cloned);
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::getColumn(int $col): Vector
 *
 * Matches the PHP package's Matrix::getColumn().
 */
PHP_METHOD(OceanMoon_Math_Matrix, getColumn)
{
	zend_long col;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(col)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);
	if (col < 0 || col >= column_count) {
		matrix_throw_index_error("column", col, column_count);
		RETURN_THROWS();
	}

	double *values = matrix_read_all(self);
	if (values == NULL) {
		RETURN_THROWS();
	}
	double *column = safe_emalloc((size_t) (row_count > 0 ? row_count : 1), sizeof(double), 0);
	for (zend_long i = 0; i < row_count; i++) {
		column[i] = values[i * column_count + col];
	}

	vector_create_from(return_value, row_count, column);
	efree(values);
	efree(column);
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::copy(int $row, int $col, int $rowCount, int $colCount): Matrix
 *
 * Matches the PHP package's Matrix::copy().
 */
PHP_METHOD(OceanMoon_Math_Matrix, copy)
{
	zend_long row, col, sub_row_count, sub_col_count;

	ZEND_PARSE_PARAMETERS_START(4, 4)
		Z_PARAM_LONG(row)
		Z_PARAM_LONG(col)
		Z_PARAM_LONG(sub_row_count)
		Z_PARAM_LONG(sub_col_count)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);

	if (matrix_check_range("row", row, sub_row_count, row_count) == FAILURE
		|| matrix_check_range("column", col, sub_col_count, column_count) == FAILURE) {
		RETURN_THROWS();
	}

	double *values = matrix_read_all(self);
	if (values == NULL) {
		RETURN_THROWS();
	}
	double *sub_values = safe_emalloc(
		(size_t) (sub_row_count > 0 ? sub_row_count : 1),
		(size_t) (sub_col_count > 0 ? sub_col_count : 1) * sizeof(double), 0
	);
	for (zend_long i = 0; i < sub_row_count; i++) {
		for (zend_long j = 0; j < sub_col_count; j++) {
			sub_values[i * sub_col_count + j] = values[(row + i) * column_count + col + j];
		}
	}

	matrix_create_from(return_value, sub_row_count, sub_col_count, sub_values);
	efree(values);
	efree(sub_values);
}
/* }}} */
