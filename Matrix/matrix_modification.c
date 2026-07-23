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
#include "matrix_arginfo.h"

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
 * Matches the PHP package's Matrix::setRow(): copies $vec's elements into the row's existing
 * Vector in place, preserving the row's object identity.
 */
PHP_METHOD(OceanMoon_Math_Matrix, setRow)
{
	zend_long row;
	zval *vec;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(row)
		Z_PARAM_OBJECT_OF_CLASS(vec, vector_ce_Vector)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long row_count = matrix_read_row_count(self);
	if (row < 0 || row >= row_count) {
		zend_string *msg = strpprintf(
			0, "Invalid row index: " ZEND_LONG_FMT ". Must be in the range 0-" ZEND_LONG_FMT ".",
			row, row_count - 1
		);
		zend_throw_exception(spl_ce_OutOfRangeException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		RETURN_THROWS();
	}

	zend_object *vec_obj = Z_OBJ_P(vec);
	zend_long vector_size = vector_read_size(vec_obj);
	zend_long column_count = matrix_read_column_count(self);
	if (vector_size != column_count) {
		zend_string *msg = strpprintf(
			0, "Cannot set row due to incorrect Vector size: " ZEND_LONG_FMT ". Expected " ZEND_LONG_FMT ".",
			vector_size, column_count
		);
		zend_throw_exception(spl_ce_LengthException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		RETURN_THROWS();
	}

	for (zend_long col = 0; col < column_count; col++) {
		double value;
		vector_read_element(vec_obj, col, &value);
		if (matrix_write_element(self, row, col, value) == FAILURE) {
			RETURN_THROWS();
		}
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
		zend_string *msg = strpprintf(
			0, "Invalid column index: " ZEND_LONG_FMT ". Must be in the range 0-" ZEND_LONG_FMT ".",
			col, column_count - 1
		);
		zend_throw_exception(spl_ce_OutOfRangeException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		RETURN_THROWS();
	}

	zend_object *vec_obj = Z_OBJ_P(vec);
	zend_long vector_size = vector_read_size(vec_obj);
	zend_long row_count = matrix_read_row_count(self);
	if (vector_size != row_count) {
		zend_string *msg = strpprintf(
			0, "Cannot set column due to incorrect Vector size: " ZEND_LONG_FMT ". Expected " ZEND_LONG_FMT ".",
			vector_size, row_count
		);
		zend_throw_exception(spl_ce_LengthException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		RETURN_THROWS();
	}

	for (zend_long row = 0; row < row_count; row++) {
		double value;
		vector_read_element(vec_obj, row, &value);
		if (matrix_write_element(self, row, col, value) == FAILURE) {
			RETURN_THROWS();
		}
	}
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
	if (row < 0 || row + other_row_count > self_row_count) {
		zend_string *msg = strpprintf(
			0, "Invalid row range: " ZEND_LONG_FMT "-" ZEND_LONG_FMT ". Must be within 0-" ZEND_LONG_FMT ".",
			row, row + other_row_count, self_row_count - 1
		);
		zend_throw_exception(spl_ce_OutOfRangeException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		RETURN_THROWS();
	}

	zend_long self_column_count = matrix_read_column_count(self);
	zend_long other_column_count = matrix_read_column_count(other_obj);
	if (col < 0 || col + other_column_count > self_column_count) {
		zend_string *msg = strpprintf(
			0, "Invalid column range: " ZEND_LONG_FMT "-" ZEND_LONG_FMT ". Must be within 0-" ZEND_LONG_FMT ".",
			col, col + other_column_count, self_column_count - 1
		);
		zend_throw_exception(spl_ce_OutOfRangeException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		RETURN_THROWS();
	}

	for (zend_long i = 0; i < other_row_count; i++) {
		for (zend_long j = 0; j < other_column_count; j++) {
			double value;
			matrix_read_element(other_obj, i, j, &value);
			if (matrix_write_element(self, row + i, col + j, value) == FAILURE) {
				RETURN_THROWS();
			}
		}
	}
}
/* }}} */
