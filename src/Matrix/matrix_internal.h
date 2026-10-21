/*
 * matrix_internal.h
 *
 * Cross-file declarations shared between matrix.c and the other matrix_*.c implementation files in this directory,
 * and with the Vector files that build or multiply a Matrix. Also exposes matrix_minit() to ../oceanmoon_math.c, which
 * calls it from the module's real PHP_MINIT_FUNCTION. Not a public API - nothing here is exposed to other extensions.
 */

#ifndef PHP_MATRIX_INTERNAL_H
#define PHP_MATRIX_INTERNAL_H

#include "php.h"
#include "../Vector/vector_internal.h"

/* The registered class entry for OceanMoon\Math\Matrix. Defined in matrix.c. */
extern zend_class_entry *matrix_ce_Matrix;

/* Slot offsets of the $rowCount, $columnCount and $data properties, set in matrix_minit(). See
 * math_types_property_offset() (types.h). */
extern uint32_t matrix_offset_row_count;
extern uint32_t matrix_offset_column_count;
extern uint32_t matrix_offset_data;

/* Shared helpers, defined in matrix.c. See their doc comments there. */
zend_result matrix_init(zend_object *obj, zend_long row_count, zend_long column_count);
zend_result matrix_create(zval *return_value, zend_long row_count, zend_long column_count);
zend_long matrix_read_row_count(zend_object *obj);
zend_long matrix_read_column_count(zend_object *obj);
zend_object *matrix_read_row(zend_object *obj, zend_long row);
zend_result matrix_read_element(zend_object *obj, zend_long row, zend_long col, double *out_value);
zend_result matrix_write_element(zend_object *obj, zend_long row, zend_long col, double value);
zend_result matrix_write_result(zend_object *obj, zend_long row, zend_long col, double value, const char *operation);
double *matrix_read_all(zend_object *obj);
void matrix_create_from(zval *return_value, zend_long row_count, zend_long column_count, const double *values);

/* Index and range checks, defined in matrix_helpers.c. Used by the methods that take a row or column index. */
void matrix_throw_index_error(const char *axis, zend_long index, zend_long size);
zend_result matrix_check_range(const char *axis, zend_long start, zend_long count, zend_long size);

/* Copies a Vector into a row in place. Defined in matrix_modification.c; shared by setRow() and offsetSet()
 * (matrix_arrayaccess.c). */
zend_result matrix_set_row(zend_object *self, zend_long row, zend_object *vec);

/* Builds an identity Matrix. Defined in matrix_factory.c; shared by identity() and pow() (matrix_power.c). */
zend_result matrix_identity(zend_long size, zval *out);

/* The determinant and inverse helpers, defined in matrix_helpers.c and used by det() (matrix_linear_algebra.c) and
 * inv() (matrix_arithmetic.c). They take the list<list<float>> zval matrix_to_data_array() builds. See their doc
 * comments there. */
zend_result matrix_to_data_array(zend_object *obj, zval *out);
double matrix_calc_det(zval *data_array);
void matrix_get_minor(zend_object *obj, zend_long exclude_row, zend_long exclude_col, zval *out_array);

/* An LU decomposition with partial pivoting, from matrix_lu_decompose(). See its doc comment in matrix_helpers.c. */
typedef struct {
	zend_long n;
	double *lu;       /* n × n, row-major: U on and above the diagonal, L's multipliers below it. */
	zend_long *perm;  /* Position to original row. */
	int sign;         /* The permutation's sign, 1 or -1. */
	bool singular;
	double *col_scales;  /* Each column's largest absolute element. */
	double *row_scales;  /* Each original row's largest absolute element, divided by its column's scale. */
} matrix_lu;

void matrix_lu_decompose(zval *data_array, matrix_lu *out);
void matrix_lu_free(matrix_lu *lu);
zend_result matrix_inv_from_lu(const matrix_lu *lu, zval *return_value);
bool matrix_is_ill_conditioned(zend_object *matrix, zend_object *inverse, const matrix_lu *lu);

/* The computational cores of add()/sub()/mul()/div()/inv()/pow(), defined in matrix_arithmetic.c (pow() in
 * matrix_power.c). Shared with the operator overloads (matrix_operators.c), so operator and method results and
 * exceptions are identical. matrix_calc_mul_matrix() is also used by pow() and by Vector's mul() and outer(), and
 * matrix_calc_inv() by pow() for a negative exponent. matrix_calc_scalar_div() backs only the `/` operator's
 * `float / Matrix` form, which has no named method. */
zend_result matrix_calc_add(zend_object *self, zend_object *other, zval *return_value);
zend_result matrix_calc_sub(zend_object *self, zend_object *other, zval *return_value);
zend_result matrix_calc_mul_scalar(zend_object *self, double scalar, zval *return_value);
zend_result matrix_calc_mul_matrix(zend_object *self, zend_object *other, zval *return_value);
zend_result matrix_calc_mul_vector(zend_object *self, zend_object *vector, zval *return_value);
zend_result matrix_calc_div_scalar(zend_object *self, double scalar, zval *return_value);
zend_result matrix_calc_scalar_div(double scalar, zend_object *self, zval *return_value);
zend_result matrix_calc_inv(zend_object *self, zval *return_value);
zend_result matrix_calc_pow(zend_object *self, zend_long exponent, zval *return_value);

/* The `do_operation` object handler backing Matrix's operator overloads (+, -, *, /, **), defined
 * in matrix_operators.c. Installed on matrix_object_handlers in matrix_minit() (matrix.c). */
zend_result matrix_do_operation(uint8_t opcode, zval *result, zval *op1, zval *op2);

/* Module lifecycle hook, called from ../oceanmoon_math.c. See its doc comment in matrix.c. */
zend_result matrix_minit(void);

#endif /* PHP_MATRIX_INTERNAL_H */
