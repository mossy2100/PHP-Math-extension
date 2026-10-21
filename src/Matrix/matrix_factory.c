/*
 * matrix_factory.c
 *
 * Factory methods for OceanMoon\Math\Matrix: fromArray(), identity(). Mirrors the PHP package's
 * Matrix::fromArray() (validates the argument is a rectangular list of lists of int|float values)
 * and Matrix::identity().
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "matrix_internal.h"
#include "../../oceanmoon_math_arginfo.h"
#include "floats.h"

/* {{{ OceanMoon\Math\Matrix::fromArray(array $arr): Matrix */
PHP_METHOD(OceanMoon_Math_Matrix, fromArray)
{
	zend_array *arr;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(arr)
	ZEND_PARSE_PARAMETERS_END();

	/* Handle empty input. */
	if (zend_hash_num_elements(arr) == 0) {
		if (matrix_create(return_value, 0, 0) == FAILURE) {
			RETURN_THROWS();
		}
		return;
	}

	/* Check the outer array is a list. */
	if (!zend_array_is_list(arr)) {
		zend_throw_exception(spl_ce_DomainException, "Cannot create Matrix from array. Must be a list.", 0);
		RETURN_THROWS();
	}

	zend_long row_count = (zend_long) zend_hash_num_elements(arr);
	zend_long column_count = -1;

	zend_long row_index = 0;
	zval *row_zval;
	ZEND_HASH_FOREACH_VAL(arr, row_zval)
	{
		/* Check each row is an array. */
		if (Z_TYPE_P(row_zval) != IS_ARRAY) {
			zend_string *msg = strpprintf(
				0, "Cannot create Matrix from array. Row " ZEND_LONG_FMT " must be an array.", row_index
			);
			zend_throw_exception(spl_ce_InvalidArgumentException, ZSTR_VAL(msg), 0);
			zend_string_release(msg);
			RETURN_THROWS();
		}

		/* Check each row is a list. */
		if (!zend_array_is_list(Z_ARRVAL_P(row_zval))) {
			zend_string *msg = strpprintf(
				0, "Cannot create Matrix from array. Row " ZEND_LONG_FMT " must be a list.", row_index
			);
			zend_throw_exception(spl_ce_DomainException, ZSTR_VAL(msg), 0);
			zend_string_release(msg);
			RETURN_THROWS();
		}

		/* Check all rows have the same number of columns. */
		zend_long this_row_size = (zend_long) zend_hash_num_elements(Z_ARRVAL_P(row_zval));
		if (column_count == -1) {
			column_count = this_row_size;
		} else if (this_row_size != column_count) {
			zend_throw_exception(
				spl_ce_LengthException,
				"Cannot create Matrix from array. All rows must have the same number of elements.", 0
			);
			RETURN_THROWS();
		}

		/* Check each value is a finite number. */
		zend_long col_index = 0;
		zval *value;
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(row_zval), value)
		{
			/* Check the value is a number. */
			if (Z_TYPE_P(value) != IS_LONG && Z_TYPE_P(value) != IS_DOUBLE) {
				zend_string *msg = strpprintf(
					0, "Cannot create Matrix from array. Element at row " ZEND_LONG_FMT ", column " ZEND_LONG_FMT
					" must be an int or float.", row_index, col_index
				);
				zend_throw_exception(spl_ce_InvalidArgumentException, ZSTR_VAL(msg), 0);
				zend_string_release(msg);
				RETURN_THROWS();
			}

			/* Check the value is finite. */
			if (Z_TYPE_P(value) == IS_DOUBLE && !zend_finite(Z_DVAL_P(value))) {
				zend_string *value_str = math_floats_to_str(Z_DVAL_P(value));
				zend_string *msg = strpprintf(
					0, "Cannot create Matrix from array. Element at row " ZEND_LONG_FMT ", column " ZEND_LONG_FMT
					" (%s) must be finite.", row_index, col_index, ZSTR_VAL(value_str)
				);
				zend_string_release(value_str);
				zend_throw_exception(spl_ce_DomainException, ZSTR_VAL(msg), 0);
				zend_string_release(msg);
				RETURN_THROWS();
			}
			col_index++;
		}
		ZEND_HASH_FOREACH_END();

		row_index++;
	}
	ZEND_HASH_FOREACH_END();

	if (matrix_create(return_value, row_count, column_count) == FAILURE) {
		RETURN_THROWS();
	}
	zend_object *obj = Z_OBJ_P(return_value);

	row_index = 0;
	ZEND_HASH_FOREACH_VAL(arr, row_zval)
	{
		zend_long col_index = 0;
		zval *value;
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(row_zval), value)
		{
			double element = Z_TYPE_P(value) == IS_LONG ? (double) Z_LVAL_P(value) : Z_DVAL_P(value);
			if (matrix_write_element(obj, row_index, col_index, element) == FAILURE) {
				zval_ptr_dtor(return_value);
				ZVAL_UNDEF(return_value);
				RETURN_THROWS();
			}
			col_index++;
		}
		ZEND_HASH_FOREACH_END();

		row_index++;
	}
	ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ matrix_identity
 *
 * Builds an identity Matrix of the given size into out. Shared by identity() and pow() (matrix_power.c). Returns
 * FAILURE, with DomainException thrown, if size is negative.
 */
zend_result matrix_identity(zend_long size, zval *out)
{
	if (matrix_create(out, size, size) == FAILURE) {
		return FAILURE;
	}
	zend_object *obj = Z_OBJ_P(out);
	for (zend_long i = 0; i < size; i++) {
		if (matrix_write_element(obj, i, i, 1.0) == FAILURE) {
			zval_ptr_dtor(out);
			ZVAL_UNDEF(out);
			return FAILURE;
		}
	}
	return SUCCESS;
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::identity(int $size): Matrix */
PHP_METHOD(OceanMoon_Math_Matrix, identity)
{
	zend_long size;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(size)
	ZEND_PARSE_PARAMETERS_END();

	if (matrix_identity(size, return_value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */
