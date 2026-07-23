/*
 * matrix_helpers.c
 *
 * Private helpers for OceanMoon\Math\Matrix, not PHP-visible: matrix_calc_det() (mirrors
 * Matrix::calcDet()) and matrix_get_minor() (mirrors Matrix::getMinor()). Used by det() and inv()
 * in matrix_linear_algebra.c and matrix_arithmetic.c respectively.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "matrix_internal.h"

/* {{{ matrix_calc_det
 *
 * Recursive determinant via cofactor expansion, operating on a list<list<float>> zval (as
 * produced by toArray() or matrix_get_minor()). Mirrors Matrix::calcDet() exactly, including its
 * closed-form 1x1/2x2/3x3 (Sarrus' Rule) fast paths.
 */
double matrix_calc_det(zval *data_array)
{
	HashTable *ht = Z_ARRVAL_P(data_array);
	zend_long n = (zend_long) zend_hash_num_elements(ht);

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

	/* Cofactor expansion for larger matrices. */
	zval *row0 = zend_hash_index_find(ht, 0);
	double det = 0.0;
	for (zend_long j = 0; j < n; j++) {
		zval submatrix;
		array_init_size(&submatrix, (uint32_t) (n - 1));
		for (zend_long i = 1; i < n; i++) {
			zval *row = zend_hash_index_find(ht, (zend_ulong) i);
			zval sub_row;
			array_init_size(&sub_row, (uint32_t) (n - 1));
			for (zend_long k = 0; k < n; k++) {
				if (k != j) {
					zval *value = zend_hash_index_find(Z_ARRVAL_P(row), (zend_ulong) k);
					zval copy;
					ZVAL_COPY_VALUE(&copy, value);
					zend_hash_next_index_insert(Z_ARRVAL(sub_row), &copy);
				}
			}
			zend_hash_next_index_insert(Z_ARRVAL(submatrix), &sub_row);
		}

		double a0j = Z_DVAL_P(zend_hash_index_find(Z_ARRVAL_P(row0), (zend_ulong) j));
		double cofactor = (j % 2 == 0 ? 1.0 : -1.0) * a0j * matrix_calc_det(&submatrix);
		det += cofactor;

		zval_ptr_dtor(&submatrix);
	}

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
