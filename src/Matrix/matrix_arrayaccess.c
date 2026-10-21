/*
 * matrix_arrayaccess.c
 *
 * ArrayAccess methods for OceanMoon\Math\Matrix: offsetExists(), offsetGet(), offsetSet(),
 * offsetUnset(). Mirrors the "ArrayAccess methods" region of the PHP package's Matrix class -
 * offsetGet() returns the Matrix's actual internal row Vector (a live reference, unlike getRow()'s
 * independent copy), which is what makes `$m[$row][$col] = $x` work. Also getIterator(), from the
 * "IteratorAggregate implementation" region, which yields the same live rows.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "matrix_internal.h"
#include "../../oceanmoon_math_arginfo.h"
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
		matrix_throw_index_error("row", row, row_count);
		RETURN_THROWS();
	}

	zend_object *row_obj = matrix_read_row(self, row);
	if (row_obj == NULL) {
		RETURN_THROWS();
	}
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

	if (matrix_set_row(Z_OBJ_P(ZEND_THIS), Z_LVAL_P(offset), Z_OBJ_P(value)) == FAILURE) {
		RETURN_THROWS();
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

/* {{ OceanMoon\Math\Matrix::getIterator(): Traversable
 *
 * Matches the PHP package's Matrix::getIterator(): an ArrayIterator over the rows. The $data array holds the
 * row Vector objects, so the rows are live, matching offsetGet().
 */
PHP_METHOD(OceanMoon_Math_Matrix, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zval *data = math_types_read_property(Z_OBJ_P(ZEND_THIS), matrix_offset_data);
	if (math_types_array_iterator(return_value, data) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }} */
