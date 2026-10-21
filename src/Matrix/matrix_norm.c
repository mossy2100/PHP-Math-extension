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
#include "exceptions.h"
#include "../../oceanmoon_math_arginfo.h"
#include "floats.h"

/* {{{ OceanMoon\Math\Matrix::norm(): float
 *
 * Matches the PHP package's Matrix::norm(): the Frobenius norm, i.e. the magnitude of the elements as one Vector.
 * Like vector_compute_magnitude() (Vector/vector_linear_algebra.c), it scales the elements by a power of 2 so
 * their squares can't overflow or underflow, then undoes the scaling.
 */
PHP_METHOD(OceanMoon_Math_Matrix, norm)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);
	double *values = matrix_read_all(self);
	if (values == NULL) {
		RETURN_THROWS();
	}

	/* Handle a zero or empty matrix. */
	double max = 0.0;
	for (zend_long i = 0; i < row_count; i++) {
		for (zend_long j = 0; j < column_count; j++) {
			double value = values[i * column_count + j];
			max = fmax(max, fabs(value));
		}
	}
	double scale = math_floats_power_of_2_scale(max);
	if (scale == 0.0) {
		efree(values);
		RETURN_DOUBLE(0.0);
	}

	/* Calculate the norm of the scaled matrix, then undo the scaling. */
	double sum = 0.0;
	for (zend_long i = 0; i < row_count; i++) {
		for (zend_long j = 0; j < column_count; j++) {
			double value = values[i * column_count + j];
			double scaled = value / scale;
			sum += scaled * scaled;
		}
	}

	efree(values);

	double norm = scale * sqrt(sum);
	if (!zend_finite(norm)) {
		math_throw_overflow("compute norm of the given Matrix");
		RETURN_THROWS();
	}

	RETURN_DOUBLE(norm);
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
	double *values = matrix_read_all(self);
	if (values == NULL) {
		RETURN_THROWS();
	}

	double max = 0.0;
	for (zend_long j = 0; j < column_count; j++) {
		double col_sum = 0.0;
		for (zend_long i = 0; i < row_count; i++) {
			double value = values[i * column_count + j];
			col_sum += fabs(value);
		}
		if (col_sum > max) {
			max = col_sum;
		}
	}

	efree(values);

	if (!zend_finite(max)) {
		math_throw_overflow("compute P1 norm of the given Matrix");
		RETURN_THROWS();
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
	double *values = matrix_read_all(self);
	if (values == NULL) {
		RETURN_THROWS();
	}

	double max = 0.0;
	for (zend_long i = 0; i < row_count; i++) {
		double row_sum = 0.0;
		for (zend_long j = 0; j < column_count; j++) {
			double value = values[i * column_count + j];
			row_sum += fabs(value);
		}
		if (row_sum > max) {
			max = row_sum;
		}
	}

	efree(values);

	if (!zend_finite(max)) {
		math_throw_overflow("compute P-infinity norm of the given Matrix");
		RETURN_THROWS();
	}

	RETURN_DOUBLE(max);
}
/* }}} */
