/*
 * matrix_transformation.c
 *
 * Transformation methods for OceanMoon\Math\Matrix: resize(). Mirrors the "Transformation methods"
 * region of the PHP package's Matrix class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "matrix_internal.h"
#include "matrix_arginfo.h"

/* {{{ OceanMoon\Math\Matrix::resize(int $rowCount, int $columnCount): Matrix
 *
 * Matches the PHP package's Matrix::resize(): anchored at (0, 0), zero-filling extra rows/columns
 * or dropping excess ones.
 */
PHP_METHOD(OceanMoon_Math_Matrix, resize)
{
	zend_long row_count, column_count;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(row_count)
		Z_PARAM_LONG(column_count)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long self_row_count = matrix_read_row_count(self);
	zend_long self_column_count = matrix_read_column_count(self);

	zend_long copy_row_count = MIN(self_row_count, row_count);
	zend_long copy_column_count = MIN(self_column_count, column_count);

	if (matrix_create(return_value, row_count, column_count) == FAILURE) {
		RETURN_THROWS();
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < copy_row_count; i++) {
		for (zend_long j = 0; j < copy_column_count; j++) {
			double value;
			matrix_read_element(self, i, j, &value);
			if (matrix_write_element(result, i, j, value) == FAILURE) {
				zval_ptr_dtor(return_value);
				ZVAL_UNDEF(return_value);
				RETURN_THROWS();
			}
		}
	}
}
/* }}} */
