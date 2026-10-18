/*
 * matrix_helpers.c
 *
 * Private helpers for OceanMoon\Math\Matrix, not PHP-visible: matrix_calc_det() (mirrors
 * Matrix::calcDet()), matrix_get_minor() (mirrors Matrix::getMinor()), and the LU decomposition helpers
 * matrix_lu_decompose() and matrix_inv_from_lu() (mirroring Matrix::luDecompose() and Matrix::invFromLu()). Used by
 * det() and inv() in matrix_linear_algebra.c and matrix_arithmetic.c. Also matrix_throw_index_error() and
 * matrix_check_range() (mirroring Matrix::indexMessage() and Matrix::checkRange()), for the index and range checks.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "matrix_internal.h"
#include "exceptions.h"
#include "floats.h"

#include <float.h>
#include <math.h>

/* {{{ matrix_throw_index_error
 *
 * Throws OutOfRangeException for a row or column index outside 0 to size - 1, matching the package's
 * Matrix::indexMessage(): "Invalid row index: 3. Must be in the range 0-2.", or "Invalid row index: 0. The Matrix has
 * no rows." if size is 0. axis is "row" or "column".
 */
void matrix_throw_index_error(const char *axis, zend_long index, zend_long size)
{
	zend_string *msg = size == 0
		? strpprintf(0, "Invalid %s index: " ZEND_LONG_FMT ". The Matrix has no %ss.", axis, index, axis)
		: strpprintf(
			0, "Invalid %s index: " ZEND_LONG_FMT ". Must be in the range 0-" ZEND_LONG_FMT ".", axis, index, size - 1
		);
	zend_throw_exception(spl_ce_OutOfRangeException, ZSTR_VAL(msg), 0);
	zend_string_release(msg);
}
/* }}} */

/* {{{ matrix_check_range
 *
 * Checks a range of rows or columns, given by its start and length, lies within the Matrix, matching the package's
 * Matrix::checkRange(). The start must be a valid index, even for an empty range, so a Matrix with no rows or columns
 * has no valid range. Returns FAILURE, with OutOfRangeException thrown, if the start isn't a valid index (the
 * matrix_throw_index_error() message), the count is negative ("Invalid row count: -1. Must not be negative."), or the
 * range extends beyond the Matrix ("Invalid row range: 1-3. Must be within 0-2."). axis is "row" or "column".
 */
zend_result matrix_check_range(const char *axis, zend_long start, zend_long count, zend_long size)
{
	if (start < 0 || start >= size) {
		matrix_throw_index_error(axis, start, size);
		return FAILURE;
	}

	zend_string *msg;
	if (count < 0) {
		msg = strpprintf(0, "Invalid %s count: " ZEND_LONG_FMT ". Must not be negative.", axis, count);
	} else if (count > size - start) {
		/* Add as unsigned, so a sum beyond ZEND_LONG_MAX wraps rather than being undefined behaviour. (The package
		 * shows it as a float, so the two differ only for an absurdly large count.) */
		zend_long end = (zend_long) ((zend_ulong) start + (zend_ulong) (count - 1));
		msg = strpprintf(
			0, "Invalid %s range: " ZEND_LONG_FMT "-" ZEND_LONG_FMT ". Must be within 0-" ZEND_LONG_FMT ".",
			axis, start, end, size - 1
		);
	} else {
		return SUCCESS;
	}
	zend_throw_exception(spl_ce_OutOfRangeException, ZSTR_VAL(msg), 0);
	zend_string_release(msg);
	return FAILURE;
}
/* }}} */

/* {{{ matrix_calc_det
 *
 * The determinant of a list<list<float>> zval (as produced by toArray() or matrix_get_minor()). Mirrors
 * Matrix::calcDet() exactly: closed-form formulas up to 3x3 (Sarrus' Rule for 3x3), and LU decomposition for larger
 * matrices. The result may be non-finite; the caller checks it.
 */
double matrix_calc_det(zval *data_array)
{
	HashTable *ht = Z_ARRVAL_P(data_array);
	zend_long n = (zend_long) zend_hash_num_elements(ht);

	if (n == 0) {
		/* The 0x0 determinant is the empty product, 1, so the identity and multiplicativity
		 * properties hold for empty matrices. */
		return 1.0;
	}

	if (n == 1) {
		zval *row0 = zend_hash_index_find(ht, 0);
		return Z_DVAL_P(zend_hash_index_find(Z_ARRVAL_P(row0), 0));
	}

	if (n == 2) {
		zval *row0 = zend_hash_index_find(ht, 0);
		zval *row1 = zend_hash_index_find(ht, 1);
		double a00 = Z_DVAL_P(zend_hash_index_find(Z_ARRVAL_P(row0), 0));
		double a01 = Z_DVAL_P(zend_hash_index_find(Z_ARRVAL_P(row0), 1));
		double a10 = Z_DVAL_P(zend_hash_index_find(Z_ARRVAL_P(row1), 0));
		double a11 = Z_DVAL_P(zend_hash_index_find(Z_ARRVAL_P(row1), 1));
		return a00 * a11 - a01 * a10;
	}

	if (n == 3) {
		zval *row0 = zend_hash_index_find(ht, 0);
		zval *row1 = zend_hash_index_find(ht, 1);
		zval *row2 = zend_hash_index_find(ht, 2);
		double a00 = Z_DVAL_P(zend_hash_index_find(Z_ARRVAL_P(row0), 0));
		double a01 = Z_DVAL_P(zend_hash_index_find(Z_ARRVAL_P(row0), 1));
		double a02 = Z_DVAL_P(zend_hash_index_find(Z_ARRVAL_P(row0), 2));
		double a10 = Z_DVAL_P(zend_hash_index_find(Z_ARRVAL_P(row1), 0));
		double a11 = Z_DVAL_P(zend_hash_index_find(Z_ARRVAL_P(row1), 1));
		double a12 = Z_DVAL_P(zend_hash_index_find(Z_ARRVAL_P(row1), 2));
		double a20 = Z_DVAL_P(zend_hash_index_find(Z_ARRVAL_P(row2), 0));
		double a21 = Z_DVAL_P(zend_hash_index_find(Z_ARRVAL_P(row2), 1));
		double a22 = Z_DVAL_P(zend_hash_index_find(Z_ARRVAL_P(row2), 2));
		/* Sarrus' Rule. */
		return a00 * a11 * a22 + a01 * a12 * a20 + a02 * a10 * a21
			- a02 * a11 * a20 - a00 * a12 * a21 - a01 * a10 * a22;
	}

	/* LU decomposition for larger matrices: the product of the pivots, negated for an odd number of row swaps. */
	matrix_lu lu;
	matrix_lu_decompose(data_array, &lu);
	double det = (double) lu.sign;
	for (zend_long i = 0; i < n; i++) {
		det *= lu.lu[i * n + i];
	}
	matrix_lu_free(&lu);

	return det;
}
/* }}} */

/* {{{ matrix_get_minor
 *
 * Builds the minor matrix (as a list<list<float>> zval, suitable for matrix_calc_det()) obtained
 * by removing $exclude_row and $exclude_col from the Matrix identified by $obj. Mirrors
 * Matrix::getMinor().
 */
void matrix_get_minor(zend_object *obj, zend_long exclude_row, zend_long exclude_col, zval *out_array)
{
	zend_long row_count = matrix_read_row_count(obj);
	zend_long column_count = matrix_read_column_count(obj);

	array_init_size(out_array, (uint32_t) (row_count - 1));
	for (zend_long i = 0; i < row_count; i++) {
		if (i == exclude_row) {
			continue;
		}

		zval row;
		array_init_size(&row, (uint32_t) (column_count - 1));
		for (zend_long j = 0; j < column_count; j++) {
			if (j == exclude_col) {
				continue;
			}
			double value;
			matrix_read_element(obj, i, j, &value);
			zval v;
			ZVAL_DOUBLE(&v, value);
			zend_hash_next_index_insert(Z_ARRVAL(row), &v);
		}
		zend_hash_next_index_insert(Z_ARRVAL_P(out_array), &row);
	}
}
/* }}} */

/* {{{ matrix_lu_decompose
 *
 * Decomposes a square list<list<float>> zval into PA = LU by Gaussian elimination with partial pivoting. Mirrors
 * Matrix::luDecompose() exactly, in the same operation order, so the results are bit-identical. Each step swaps in the
 * row with the largest pivot candidate. The matrix is singular if a pivot is at most n × DBL_EPSILON times the largest
 * absolute element of its original row, once each column is divided by its largest absolute element. Dividing a
 * column doesn't change the pivot choices or multipliers, so the columns are only scaled in that comparison. The caller
 * frees the result with matrix_lu_free().
 */
void matrix_lu_decompose(zval *data_array, matrix_lu *out)
{
	HashTable *ht = Z_ARRVAL_P(data_array);
	zend_long n = (zend_long) zend_hash_num_elements(ht);

	out->n = n;
	out->lu = safe_emalloc((size_t) (n > 0 ? n * n : 1), sizeof(double), 0);
	out->perm = safe_emalloc((size_t) (n > 0 ? n : 1), sizeof(zend_long), 0);
	out->sign = 1;
	out->singular = false;
	double *row_scales = safe_emalloc((size_t) (n > 0 ? n : 1), sizeof(double), 0);
	double *col_scales = safe_emalloc((size_t) (n > 0 ? n : 1), sizeof(double), 0);
	double *a = out->lu;

	/* Copy the matrix, and get the scale of each column: its largest absolute element. */
	for (zend_long j = 0; j < n; j++) {
		col_scales[j] = 0.0;
	}
	for (zend_long i = 0; i < n; i++) {
		zval *row = zend_hash_index_find(ht, (zend_ulong) i);
		for (zend_long j = 0; j < n; j++) {
			double value = Z_DVAL_P(zend_hash_index_find(Z_ARRVAL_P(row), (zend_ulong) j));
			a[i * n + j] = value;
			col_scales[j] = fmax(col_scales[j], fabs(value));
		}
		out->perm[i] = i;
	}

	/* Get the scale of each original row: its largest absolute element, with each divided by its column's scale. */
	for (zend_long i = 0; i < n; i++) {
		double row_scale = 0.0;
		for (zend_long j = 0; j < n; j++) {
			if (col_scales[j] != 0.0) {
				row_scale = fmax(row_scale, fabs(a[i * n + j]) / col_scales[j]);
			}
		}
		row_scales[i] = row_scale;
	}

	for (zend_long k = 0; k < n; k++) {
		/* Find the row with the largest pivot candidate. */
		zend_long pivot_row = k;
		double max = fabs(a[k * n + k]);
		for (zend_long i = k + 1; i < n; i++) {
			if (fabs(a[i * n + k]) > max) {
				max = fabs(a[i * n + k]);
				pivot_row = i;
			}
		}

		/* Swap it into place. */
		if (pivot_row != k) {
			for (zend_long j = 0; j < n; j++) {
				double tmp = a[k * n + j];
				a[k * n + j] = a[pivot_row * n + j];
				a[pivot_row * n + j] = tmp;
			}
			zend_long index = out->perm[k];
			out->perm[k] = out->perm[pivot_row];
			out->perm[pivot_row] = index;
			out->sign = -out->sign;
		}

		/* Check for a negligible pivot, divided by its column's scale like the row's elements. A zero column makes the
		 * matrix singular. */
		double pivot = a[k * n + k];
		if (col_scales[k] == 0.0
			|| fabs(pivot) / col_scales[k] <= (double) n * DBL_EPSILON * row_scales[out->perm[k]]) {
			out->singular = true;
		}

		/* A zero pivot means the rest of the column is zero too, so there's nothing to eliminate. */
		if (pivot == 0.0) {
			continue;
		}

		/* Eliminate the column below the pivot. */
		for (zend_long i = k + 1; i < n; i++) {
			double multiplier = a[i * n + k] / pivot;
			a[i * n + k] = multiplier;
			for (zend_long j = k + 1; j < n; j++) {
				a[i * n + j] -= multiplier * a[k * n + j];
			}
		}
	}

	efree(row_scales);
	efree(col_scales);
}
/* }}} */

/* {{{ matrix_lu_free */
void matrix_lu_free(matrix_lu *lu)
{
	efree(lu->lu);
	efree(lu->perm);
}
/* }}} */

/* {{{ matrix_inv_from_lu
 *
 * The inverse of a non-singular matrix from its LU decomposition. Mirrors Matrix::invFromLu(): each column of the
 * inverse solves Ly = Pe by forward substitution, then Ux = y by back substitution. Returns FAILURE (with
 * OverflowException already thrown) if an element of the result isn't finite.
 */
zend_result matrix_inv_from_lu(const matrix_lu *lu, zval *return_value)
{
	zend_long n = lu->n;
	const double *a = lu->lu;

	if (matrix_create(return_value, n, n) == FAILURE) {
		return FAILURE;
	}
	zend_object *inverse = Z_OBJ_P(return_value);

	double *y = safe_emalloc((size_t) (n > 0 ? n : 1), sizeof(double), 0);
	double *x = safe_emalloc((size_t) (n > 0 ? n : 1), sizeof(double), 0);

	for (zend_long col = 0; col < n; col++) {
		/* Solve Ly = Pe, where e is this column of the identity. */
		for (zend_long i = 0; i < n; i++) {
			double sum = lu->perm[i] == col ? 1.0 : 0.0;
			for (zend_long j = 0; j < i; j++) {
				sum -= a[i * n + j] * y[j];
			}
			y[i] = sum;
		}

		/* Solve Ux = y. */
		for (zend_long i = n - 1; i >= 0; i--) {
			double sum = y[i];
			for (zend_long j = i + 1; j < n; j++) {
				sum -= a[i * n + j] * x[j];
			}
			x[i] = sum / a[i * n + i];
		}

		/* Store the column. Normalize, as the substitutions can give -0.0. */
		for (zend_long i = 0; i < n; i++) {
			if (matrix_write_result(
				inverse, i, col, math_floats_normalize_zero(x[i]), "invert the given Matrix"
			) == FAILURE) {
				efree(y);
				efree(x);
				zval_ptr_dtor(return_value);
				ZVAL_UNDEF(return_value);
				return FAILURE;
			}
		}
	}

	efree(y);
	efree(x);
	return SUCCESS;
}
/* }}} */
