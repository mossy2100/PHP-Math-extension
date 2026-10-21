/*
 * matrix_linear_algebra.c
 *
 * Linear algebra methods for OceanMoon\Math\Matrix: t(), det(), trace(). Mirrors the
 * "Linear algebra methods" region of the PHP package's Matrix class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "matrix_internal.h"
#include "exceptions.h"
#include "../../oceanmoon_math_arginfo.h"

/* {{{ OceanMoon\Math\Matrix::t(): Matrix
 *
 * Matches the PHP package's Matrix::t(): the transpose.
 */
PHP_METHOD(OceanMoon_Math_Matrix, t)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);

	double *values = matrix_read_all(self);
	if (values == NULL) {
		RETURN_THROWS();
	}
	double *transposed = safe_emalloc(
		(size_t) (row_count > 0 ? row_count : 1), (size_t) (column_count > 0 ? column_count : 1) * sizeof(double), 0
	);
	for (zend_long i = 0; i < row_count; i++) {
		for (zend_long j = 0; j < column_count; j++) {
			transposed[j * row_count + i] = values[i * column_count + j];
		}
	}

	matrix_create_from(return_value, column_count, row_count, transposed);
	efree(values);
	efree(transposed);
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::det(): float
 *
 * Matches the PHP package's Matrix::det(): `$this->calcDet($this->toArray())`.
 */
PHP_METHOD(OceanMoon_Math_Matrix, det)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);

	if (row_count != column_count) {
		zend_throw_exception(spl_ce_DomainException, "Cannot compute determinant of non-square Matrix.", 0);
		RETURN_THROWS();
	}

	zval data_array;
	if (matrix_to_data_array(self, &data_array) == FAILURE) {
		RETURN_THROWS();
	}
	double det = matrix_calc_det(&data_array);
	zval_ptr_dtor(&data_array);

	if (!zend_finite(det)) {
		math_throw_overflow("compute determinant of the given Matrix");
		RETURN_THROWS();
	}

	RETURN_DOUBLE(det);
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::trace(): float
 *
 * Matches the PHP package's Matrix::trace(): sum of diagonal elements.
 */
PHP_METHOD(OceanMoon_Math_Matrix, trace)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);

	if (row_count != column_count) {
		zend_throw_exception(spl_ce_DomainException, "Cannot compute trace of non-square Matrix.", 0);
		RETURN_THROWS();
	}

	double *values = matrix_read_all(self);
	if (values == NULL) {
		RETURN_THROWS();
	}
	double sum = 0.0;
	for (zend_long i = 0; i < row_count; i++) {
		sum += values[i * row_count + i];
	}
	efree(values);

	if (!zend_finite(sum)) {
		math_throw_overflow("compute trace of the given Matrix");
		RETURN_THROWS();
	}

	RETURN_DOUBLE(sum);
}
/* }}} */
