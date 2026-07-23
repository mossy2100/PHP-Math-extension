/*
 * vector_internal.h
 *
 * Cross-file declarations shared between vector.c and the other vector_*.c implementation files
 * in this directory. Also exposes vector_minit() to ../math.c, which calls it from the module's
 * real PHP_MINIT_FUNCTION. Not a public API -- nothing here is exposed to other extensions.
 */

#ifndef PHP_VECTOR_INTERNAL_H
#define PHP_VECTOR_INTERNAL_H

#include "php.h"

/* The registered class entry for OceanMoon\Math\Vector. Defined in vector.c. */
extern zend_class_entry *vector_ce_Vector;

/* Shared helpers, defined in vector.c. See their doc comments there. */
zend_result vector_init(zend_object *obj, zend_long size);
zend_result vector_create(zval *return_value, zend_long size);
zend_long vector_read_size(zend_object *obj);
zend_result vector_read_element(zend_object *obj, zend_long index, double *out_value);
zend_result vector_write_element(zend_object *obj, zend_long index, double value);

/* Installs the magnitude computed-property object handler. Defined in vector_properties.c;
 * called from vector_minit() (vector.c). */
zend_result vector_properties_minit(void);

/* The computational cores of add()/sub()/mul()/div(), defined in vector_arithmetic.c. Shared with
 * the operator overloads (vector_operators.c) so operator and method results/exceptions are
 * guaranteed identical; vector_calc_div() is also shared with normalized()/normalize()
 * (vector_linear_algebra.c/vector_modification.c). */
zend_result vector_calc_add(zend_object *self, zend_object *other, zval *return_value);
zend_result vector_calc_sub(zend_object *self, zend_object *other, zval *return_value);
zend_result vector_calc_mul_scalar(zend_object *self, double scalar, zval *return_value);
zend_result vector_calc_mul_matrix(zend_object *self, zend_object *other, zval *return_value);
zend_result vector_calc_div(zend_object *obj, double scalar, zval *return_value);

/* Builds a 1xN / Nx1 Matrix from this Vector's elements, defined in vector_conversion.c. Shared
 * between toRowMatrix()/toColumnMatrix() themselves and mul()'s Matrix branch (vector_arithmetic.c)
 * / outer() (vector_linear_algebra.c). */
zend_result vector_to_row_matrix(zend_object *obj, zval *return_value);
zend_result vector_to_column_matrix(zend_object *obj, zval *return_value);

/* The `do_operation` object handler backing Vector's operator overloads (+, -, *, /), defined in
 * vector_operators.c. Installed on vector_object_handlers in vector_properties_minit()
 * (vector_properties.c). */
zend_result vector_do_operation(uint8_t opcode, zval *result, zval *op1, zval *op2);

/* Module lifecycle hook, called from ../math.c. See its doc comment in vector.c. */
zend_result vector_minit(void);

#endif /* PHP_VECTOR_INTERNAL_H */
