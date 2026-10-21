/*
 * matrix_modification.c
 *
 * Modification methods for OceanMoon\Math\Matrix: set(), setRow(), setColumn(), paste(). Mirrors
 * the "Modification methods" region of the PHP package's Matrix class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "matrix_internal.h"
#include "../../oceanmoon_math_arginfo.h"

/* {{{ matrix_set_row
 *
 * Copies vec's elements into the row's existing Vector in place, preserving the row's object identity. Shared by
 * setRow() and offsetSet() (matrix_arrayaccess.c). Returns FAILURE, with an exception thrown, if the row index is out
 * of range or vec's count doesn't match the column count.
 */
zend_result matrix_set_row(zend_object *self, zend_long row, zend_object *vec)
{
	zend_long row_count = matrix_read_row_count(self);
	if (row < 0 || row >= row_count) {
		matrix_throw_index_error("row", row, row_count);
		return FAILURE;
	}

	zend_long vector_count = vector_read_count(vec);
	zend_long column_count = matrix_read_column_count(self);
	if (vector_count != column_count) {
		zend_string *msg = strpprintf(
			0, "Cannot set row due to incorrect Vector count: " ZEND_LONG_FMT ". Expected " ZEND_LONG_FMT ".",
			vector_count, column_count
		);
		zend_throw_exception(spl_ce_LengthException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	for (zend_long col = 0; col < column_count; col++) {
		double value;
		if (vector_read_element(vec, col, &value) == FAILURE
			|| matrix_write_element(self, row, col, value) == FAILURE) {
			return FAILURE;
		}
	}

	return SUCCESS;
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::set(int $row, int $col, float $value): void
 *
 * Matches the PHP package's Matrix::set().
 */
PHP_METHOD(OceanMoon_Math_Matrix, set)
{
	zend_long row, col;
	double value;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_LONG(row)
		Z_PARAM_LONG(col)
		Z_PARAM_DOUBLE(value)
	ZEND_PARSE_PARAMETERS_END();

	if (matrix_write_element(Z_OBJ_P(ZEND_THIS), row, col, value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::setRow(int $row, Vector $vec): void
 *
 * Matches the PHP package's Matrix::setRow(). See matrix_set_row().
 */
PHP_METHOD(OceanMoon_Math_Matrix, setRow)
{
	zend_long row;
	zval *vec;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(row)
		Z_PARAM_OBJECT_OF_CLASS(vec, vector_ce_Vector)
	ZEND_PARSE_PARAMETERS_END();

	if (matrix_set_row(Z_OBJ_P(ZEND_THIS), row, Z_OBJ_P(vec)) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::setColumn(int $col, Vector $vec): void
 *
 * Matches the PHP package's Matrix::setColumn().
 */
PHP_METHOD(OceanMoon_Math_Matrix, setColumn)
{
	zend_long col;
	zval *vec;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(col)
		Z_PARAM_OBJECT_OF_CLASS(vec, vector_ce_Vector)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long column_count = matrix_read_column_count(self);
	if (col < 0 || col >= column_count) {
		matrix_throw_index_error("column", col, column_count);
		RETURN_THROWS();
	}

	zend_object *vec_obj = Z_OBJ_P(vec);
	zend_long vector_count = vector_read_count(vec_obj);
	zend_long row_count = matrix_read_row_count(self);
	if (vector_count != row_count) {
		zend_string *msg = strpprintf(
			0, "Cannot set column due to incorrect Vector count: " ZEND_LONG_FMT ". Expected " ZEND_LONG_FMT ".",
			vector_count, row_count
		);
		zend_throw_exception(spl_ce_LengthException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		RETURN_THROWS();
	}

	double *values = vector_read_all(vec_obj);
	if (values == NULL) {
		RETURN_THROWS();
	}
	for (zend_long row = 0; row < row_count; row++) {
		if (matrix_write_element(self, row, col, values[row]) == FAILURE) {
			break;
		}
	}
	efree(values);
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::paste(Matrix $other, int $row = 0, int $col = 0): void
 *
 * Matches the PHP package's Matrix::paste(): mutates this Matrix in place, matching set().
 */
PHP_METHOD(OceanMoon_Math_Matrix, paste)
{
	zval *other;
	zend_long row = 0, col = 0;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_OBJECT_OF_CLASS(other, matrix_ce_Matrix)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(row)
		Z_PARAM_LONG(col)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_object *other_obj = Z_OBJ_P(other);

	zend_long self_row_count = matrix_read_row_count(self);
	zend_long other_row_count = matrix_read_row_count(other_obj);
	if (matrix_check_range("row", row, other_row_count, self_row_count) == FAILURE) {
		RETURN_THROWS();
	}

	zend_long self_column_count = matrix_read_column_count(self);
	zend_long other_column_count = matrix_read_column_count(other_obj);
	if (matrix_check_range("column", col, other_column_count, self_column_count) == FAILURE) {
		RETURN_THROWS();
	}

	double *values = matrix_read_all(other_obj);
	if (values == NULL) {
		RETURN_THROWS();
	}
	for (zend_long i = 0; i < other_row_count && !EG(exception); i++) {
		for (zend_long j = 0; j < other_column_count; j++) {
			if (matrix_write_element(self, row + i, col + j, values[i * other_column_count + j]) == FAILURE) {
				break;
			}
		}
	}
	efree(values);
}
/* }}} */
