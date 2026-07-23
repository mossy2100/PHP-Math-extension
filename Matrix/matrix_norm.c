/*
 * matrix_norm.c
 *
 * Norm methods for OceanMoon\Math\Matrix: norm(), p1Norm(), pInfNorm(). Mirrors the "Norm methods"
 * region of the PHP package's Matrix class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <math.h>

#include "php.h"
#include "matrix_internal.h"
#include "matrix_arginfo.h"

/* {{{ OceanMoon\Math\Matrix::norm(): float
 *
 * Matches the PHP package's Matrix::norm(): the Frobenius norm.
 */
PHP_METHOD(OceanMoon_Math_Matrix, norm)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);

	double sum = 0.0;
	for (zend_long i = 0; i < row_count; i++) {
		for (zend_long j = 0; j < column_count; j++) {
			double value;
			matrix_read_element(self, i, j, &value);
			sum += value * value;
		}
	}

	RETURN_DOUBLE(sqrt(sum));
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::p1Norm(): float
 *
 * Matches the PHP package's Matrix::p1Norm(): maximum absolute column sum.
 */
PHP_METHOD(OceanMoon_Math_Matrix, p1Norm)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);

	double max = 0.0;
	for (zend_long j = 0; j < column_count; j++) {
		double col_sum = 0.0;
		for (zend_long i = 0; i < row_count; i++) {
			double value;
			matrix_read_element(self, i, j, &value);
			col_sum += fabs(value);
		}
		if (col_sum > max) {
			max = col_sum;
		}
	}

	RETURN_DOUBLE(max);
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::pInfNorm(): float
 *
 * Matches the PHP package's Matrix::pInfNorm(): maximum absolute row sum.
 */
PHP_METHOD(OceanMoon_Math_Matrix, pInfNorm)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);

	double max = 0.0;
	for (zend_long i = 0; i < row_count; i++) {
		double row_sum = 0.0;
		for (zend_long j = 0; j < column_count; j++) {
			double value;
			matrix_read_element(self, i, j, &value);
			row_sum += fabs(value);
		}
		if (row_sum > max) {
			max = row_sum;
		}
	}

	RETURN_DOUBLE(max);
}
/* }}} */
