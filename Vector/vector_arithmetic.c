/*
 * vector_arithmetic.c
 *
 * Unary and binary arithmetic methods for OceanMoon\Math\Vector: neg(), reciprocal(), add(),
 * sub(), mul(), div(), hadamardMul(), hadamardDiv(). Mirrors the "Unary arithmetic methods"/
 * "Binary arithmetic methods" regions of the PHP package's Vector class.
 *
 * vector_calc_div() is factored out here (rather than inlined in the div() method body) because
 * it's shared with normalized()/normalize() (vector_linear_algebra.c/vector_modification.c),
 * matching the "shared calc core" pattern used throughout this extension (e.g. Rational's
 * pow()/operator sharing rational_calc_pow()).
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "vector_internal.h"
#include "vector_arginfo.h"
#include "../Matrix/matrix_internal.h"
#include "exceptions.h"

/* {{{ vector_check_same_size
 *
 * Shared by add()/sub()/hadamardMul()/hadamardDiv(): checks $other has the same size as self,
 * throwing LengthException with the given operation-specific message otherwise.
 */
static zend_result vector_check_same_size(zend_object *self, zend_object *other, const char *verb)
{
	zend_long self_size = vector_read_size(self);
	zend_long other_size = vector_read_size(other);

	if (self_size != other_size) {
		zend_string *msg = strpprintf(
			0, "Cannot %s Vector of incorrect size: " ZEND_LONG_FMT ". Expected " ZEND_LONG_FMT ".",
			verb, other_size, self_size
		);
		zend_throw_exception(spl_ce_LengthException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::neg(): Vector
 *
 * Matches the PHP package's Vector::neg(): mul(-1).
 */
PHP_METHOD(OceanMoon_Math_Vector, neg)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long size = vector_read_size(self);

	if (vector_create(return_value, size) == FAILURE) {
		RETURN_THROWS();
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < size; i++) {
		double element;
		vector_read_element(self, i, &element);
		if (vector_write_element(result, i, -element) == FAILURE) {
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			RETURN_THROWS();
		}
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::reciprocal(): Vector
 *
 * Matches the PHP package's Vector::reciprocal(): element-wise reciprocal.
 */
PHP_METHOD(OceanMoon_Math_Vector, reciprocal)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long size = vector_read_size(self);

	if (vector_create(return_value, size) == FAILURE) {
		RETURN_THROWS();
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < size; i++) {
		double element;
		vector_read_element(self, i, &element);

		if (element == 0.0) {
			zend_string *msg = strpprintf(0, "Cannot compute reciprocal of zero at index " ZEND_LONG_FMT ".", i);
			zend_throw_exception(math_ce_ArithmeticException, ZSTR_VAL(msg), 0);
			zend_string_release(msg);
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			RETURN_THROWS();
		}

		if (vector_write_element(result, i, 1.0 / element) == FAILURE) {
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			RETURN_THROWS();
		}
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::add(Vector $other): Vector
 *
 * Matches the PHP package's Vector::add().
 */
PHP_METHOD(OceanMoon_Math_Vector, add)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(other, vector_ce_Vector)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_object *other_obj = Z_OBJ_P(other);

	if (vector_check_same_size(self, other_obj, "add") == FAILURE) {
		RETURN_THROWS();
	}

	zend_long size = vector_read_size(self);
	if (vector_create(return_value, size) == FAILURE) {
		RETURN_THROWS();
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < size; i++) {
		double a, b;
		vector_read_element(self, i, &a);
		vector_read_element(other_obj, i, &b);
		if (vector_write_element(result, i, a + b) == FAILURE) {
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			RETURN_THROWS();
		}
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::sub(Vector $other): Vector
 *
 * Matches the PHP package's Vector::sub().
 */
PHP_METHOD(OceanMoon_Math_Vector, sub)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(other, vector_ce_Vector)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_object *other_obj = Z_OBJ_P(other);

	if (vector_check_same_size(self, other_obj, "subtract") == FAILURE) {
		RETURN_THROWS();
	}

	zend_long size = vector_read_size(self);
	if (vector_create(return_value, size) == FAILURE) {
		RETURN_THROWS();
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < size; i++) {
		double a, b;
		vector_read_element(self, i, &a);
		vector_read_element(other_obj, i, &b);
		if (vector_write_element(result, i, a - b) == FAILURE) {
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			RETURN_THROWS();
		}
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::mul(float|Matrix $other): Vector
 *
 * Matches the PHP package's Vector::mul(): scalar branch scales every element; Matrix branch
 * converts this Vector to a row Matrix, multiplies by $other, and extracts the (only) resulting
 * row as a Vector -- `$this->toRowMatrix()->mul($other)->getRow(0)`.
 */
PHP_METHOD(OceanMoon_Math_Vector, mul)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(other)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long size = vector_read_size(self);

	if (Z_TYPE_P(other) == IS_DOUBLE || Z_TYPE_P(other) == IS_LONG) {
		double scalar = Z_TYPE_P(other) == IS_DOUBLE ? Z_DVAL_P(other) : (double) Z_LVAL_P(other);

		if (vector_create(return_value, size) == FAILURE) {
			RETURN_THROWS();
		}
		zend_object *result = Z_OBJ_P(return_value);

		for (zend_long i = 0; i < size; i++) {
			double element;
			vector_read_element(self, i, &element);
			if (vector_write_element(result, i, element * scalar) == FAILURE) {
				zval_ptr_dtor(return_value);
				ZVAL_UNDEF(return_value);
				RETURN_THROWS();
			}
		}
		return;
	}

	if (Z_TYPE_P(other) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(other), matrix_ce_Matrix)) {
		zend_argument_type_error(1, "must be of type float|Matrix, %s given", zend_zval_type_name(other));
		RETURN_THROWS();
	}

	zval row_matrix;
	if (vector_to_row_matrix(self, &row_matrix) == FAILURE) {
		RETURN_THROWS();
	}

	zval product;
	zend_result mul_result = matrix_calc_mul_matrix(Z_OBJ(row_matrix), Z_OBJ_P(other), &product);
	zval_ptr_dtor(&row_matrix);
	if (mul_result == FAILURE) {
		RETURN_THROWS();
	}

	zend_object *row0 = matrix_read_row(Z_OBJ(product), 0);
	zend_object *cloned = row0->handlers->clone_obj(row0);
	zval_ptr_dtor(&product);

	RETVAL_OBJ(cloned);
}
/* }}} */

/* {{{ vector_calc_div
 *
 * The computational core of div(), shared with normalized()/normalize()
 * (vector_linear_algebra.c/vector_modification.c). Matches the PHP package's Vector::div().
 */
zend_result vector_calc_div(zend_object *obj, double scalar, zval *return_value)
{
	if (scalar == 0.0) {
		zend_throw_exception(math_ce_ArithmeticException, "Cannot divide by zero.", 0);
		return FAILURE;
	}

	zend_long size = vector_read_size(obj);
	if (vector_create(return_value, size) == FAILURE) {
		return FAILURE;
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < size; i++) {
		double element;
		vector_read_element(obj, i, &element);
		if (vector_write_element(result, i, element / scalar) == FAILURE) {
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			return FAILURE;
		}
	}

	return SUCCESS;
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::div(float $scalar): Vector */
PHP_METHOD(OceanMoon_Math_Vector, div)
{
	double scalar;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(scalar)
	ZEND_PARSE_PARAMETERS_END();

	if (vector_calc_div(Z_OBJ_P(ZEND_THIS), scalar, return_value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::hadamardMul(Vector $other): Vector
 *
 * Matches the PHP package's Vector::hadamardMul(): element-wise product.
 */
PHP_METHOD(OceanMoon_Math_Vector, hadamardMul)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(other, vector_ce_Vector)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_object *other_obj = Z_OBJ_P(other);

	if (vector_check_same_size(self, other_obj, "compute Hadamard product with") == FAILURE) {
		RETURN_THROWS();
	}

	zend_long size = vector_read_size(self);
	if (vector_create(return_value, size) == FAILURE) {
		RETURN_THROWS();
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < size; i++) {
		double a, b;
		vector_read_element(self, i, &a);
		vector_read_element(other_obj, i, &b);
		if (vector_write_element(result, i, a * b) == FAILURE) {
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			RETURN_THROWS();
		}
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::hadamardDiv(Vector $other): Vector
 *
 * Matches the PHP package's Vector::hadamardDiv(): element-wise quotient.
 */
PHP_METHOD(OceanMoon_Math_Vector, hadamardDiv)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(other, vector_ce_Vector)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_object *other_obj = Z_OBJ_P(other);

	if (vector_check_same_size(self, other_obj, "compute Hadamard quotient with") == FAILURE) {
		RETURN_THROWS();
	}

	zend_long size = vector_read_size(self);
	if (vector_create(return_value, size) == FAILURE) {
		RETURN_THROWS();
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < size; i++) {
		double a, b;
		vector_read_element(self, i, &a);
		vector_read_element(other_obj, i, &b);

		if (b == 0.0) {
			zend_string *msg = strpprintf(0, "Cannot divide by zero at index " ZEND_LONG_FMT ".", i);
			zend_throw_exception(math_ce_ArithmeticException, ZSTR_VAL(msg), 0);
			zend_string_release(msg);
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			RETURN_THROWS();
		}

		if (vector_write_element(result, i, a / b) == FAILURE) {
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			RETURN_THROWS();
		}
	}
}
/* }}} */
