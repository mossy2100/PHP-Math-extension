#ifndef PHP_MATRIX_INTERNAL_H
#define PHP_MATRIX_INTERNAL_H

#include "php.h"
#include "../Vector/vector_internal.h"

extern zend_class_entry *matrix_ce_Matrix;

zend_result matrix_init(zend_object *obj, zend_long row_count, zend_long column_count);
zend_result matrix_create(zval *return_value, zend_long row_count, zend_long column_count);
zend_long matrix_read_row_count(zend_object *obj);
zend_long matrix_read_column_count(zend_object *obj);
zend_object *matrix_read_row(zend_object *obj, zend_long row);
zend_result matrix_read_element(zend_object *obj, zend_long row, zend_long col, double *out_value);
zend_result matrix_write_element(zend_object *obj, zend_long row, zend_long col, double value);

double matrix_calc_det(zval *data_array);
void matrix_get_minor(zend_object *obj, zend_long exclude_row, zend_long exclude_col, zval *out_array);

zend_result matrix_calc_mul_matrix(zend_object *self, zend_object *other, zval *return_value);
zend_result matrix_calc_inv(zend_object *self, zval *return_value);

zend_result matrix_minit(void);

#endif /* PHP_MATRIX_INTERNAL_H */
