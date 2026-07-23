/*
 * matrix_arrayaccess.c
 *
 * ArrayAccess methods for OceanMoon\Math\Matrix: offsetExists(), offsetGet(), offsetSet(),
 * offsetUnset(). Mirrors the "ArrayAccess methods" region of the PHP package's Matrix class --
 * offsetGet() returns the Matrix's actual internal row Vector (a live reference, unlike getRow()'s
 * independent copy), which is what makes `$m[$row][$col] = $x` work.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "matrix_internal.h"
#include "matrix_arginfo.h"
#include "types.h"

/* {{{ OceanMoon\Math\Matrix::offsetExists(mixed $offset): bool
 *
 * Matches the PHP package's Matrix::offsetExists().
 */
PHP_METHOD(OceanMoon_Math_Matrix, offsetExists)
{
	zval *offset;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(offset) != IS_LONG) {
		RETURN_FALSE;
	}

	zend_long row = Z_LVAL_P(offset);
	zend_long row_count = matrix_read_row_count(Z_OBJ_P(ZEND_THIS));
	RETURN_BOOL(row >= 0 && row < row_count);
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::offsetGet(mixed $offset): Vector
 *
 * Matches the PHP package's Matrix::offsetGet(): returns the live row Vector, not a copy.
 */
PHP_METHOD(OceanMoon_Math_Matrix, offsetGet)
{
	zval *offset;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(offset) != IS_LONG) {
		zend_string *msg = strpprintf(
			0, "Invalid index type: %s. Must be int.", math_types_debug_type_name(offset));
		zend_throw_exception(spl_ce_InvalidArgumentException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		RETURN_THROWS();
	}

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long row = Z_LVAL_P(offset);
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

	zend_object *row_obj = matrix_read_row(self, row);
	GC_ADDREF(row_obj);
	RETVAL_OBJ(row_obj);
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::offsetSet(mixed $offset, mixed $value): void
 *
 * Matches the PHP package's Matrix::offsetSet(): equivalent to setRow().
 */
PHP_METHOD(OceanMoon_Math_Matrix, offsetSet)
{
	zval *offset, *value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(offset)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(offset) != IS_LONG) {
		zend_string *msg = strpprintf(
			0, "Invalid index type: %s. Must be int.", math_types_debug_type_name(offset));
		zend_throw_exception(spl_ce_InvalidArgumentException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		RETURN_THROWS();
	}

	if (Z_TYPE_P(value) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(value), vector_ce_Vector)) {
		zend_string *msg = strpprintf(
			0, "Invalid row type: %s. Must be Vector.", math_types_debug_type_name(value));
		zend_throw_exception(spl_ce_InvalidArgumentException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		RETURN_THROWS();
	}

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long row = Z_LVAL_P(offset);
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

	zend_object *vec_obj = Z_OBJ_P(value);
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
		double element;
		vector_read_element(vec_obj, col, &element);
		if (matrix_write_element(self, row, col, element) == FAILURE) {
			RETURN_THROWS();
		}
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::offsetUnset(mixed $offset): void
 *
 * Matches the PHP package's Matrix::offsetUnset(): always throws, since Matrix has a fixed number
 * of rows.
 */
PHP_METHOD(OceanMoon_Math_Matrix, offsetUnset)
{
	zval *offset;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset)
	ZEND_PARSE_PARAMETERS_END();

	zend_throw_exception(spl_ce_LogicException, "Cannot unset Matrix rows.", 0);
	RETURN_THROWS();
}
/* }}} */
