/*
 * matrix_aggregation.c
 *
 * Aggregation methods for OceanMoon\Math\Matrix: count(). Mirrors the "Aggregation methods" region
 * of the PHP package's Matrix class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "matrix_internal.h"
#include "matrix_arginfo.h"

/* {{{ OceanMoon\Math\Matrix::count(): int
 *
 * Matches the PHP package's Matrix::count() (Countable): returns $rowCount * $columnCount.
 */
PHP_METHOD(OceanMoon_Math_Matrix, count)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	RETURN_LONG(matrix_read_row_count(self) * matrix_read_column_count(self));
}
/* }}} */
