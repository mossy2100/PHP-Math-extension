/*
 * vector_arithmetic.c
 *
 * Unary and binary arithmetic methods for OceanMoon\Math\Vector: neg(), hadamardInv(), add(),
 * sub(), mul(), div(), hadamardMul(), hadamardDiv(). Mirrors the "Unary arithmetic methods"/
 * "Binary arithmetic methods" regions of the PHP package's Vector class.
 *
 * vector_calc_div() is factored out here (rather than inlined in the div() method body) because
 * it's shared with normalize() (vector_linear_algebra.c), matching the "shared calc core" pattern
 * used throughout this extension (e.g. Rational's pow()/operator sharing rational_calc_pow()).
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "vector_internal.h"
#include "../../oceanmoon_math_arginfo.h"
#include "../Matrix/matrix_internal.h"
#include "exceptions.h"
#include "floats.h"

/* {{{ vector_check_same_count
 *
 * Shared by add()/sub() and hadamardMul()/hadamardDiv(): checks $other has the same count as self,
 * throwing LengthException with the given operation-specific message otherwise.
 */
static zend_result vector_check_same_count(zend_object *self, zend_object *other, const char *verb)
{
	zend_long self_count = vector_read_count(self);
	zend_long other_count = vector_read_count(other);

	if (self_count != other_count) {
		zend_string *msg = strpprintf(
			0, "Cannot %s Vector of incorrect count: " ZEND_LONG_FMT ". Expected " ZEND_LONG_FMT ".",
			verb, other_count, self_count
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
	zend_long count = vector_read_count(self);

	if (vector_create(return_value, count) == FAILURE) {
		RETURN_THROWS();
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < count; i++) {
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

/* {{{ OceanMoon\Math\Vector::hadamardInv(): Vector
 *
 * Matches the PHP package's Vector::hadamardInv(): element-wise reciprocal.
 */
PHP_METHOD(OceanMoon_Math_Vector, hadamardInv)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long count = vector_read_count(self);

	if (vector_create(return_value, count) == FAILURE) {
		RETURN_THROWS();
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < count; i++) {
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

		if (vector_write_result(result, i, 1.0 / element, "compute reciprocal of the given Vector") == FAILURE) {
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			RETURN_THROWS();
		}
	}
}
/* }}} */

/* {{{ vector_calc_add
 *
 * The computational core of add(), shared with the `+` operator (vector_operators.c). Matches the
 * PHP package's Vector::add().
 */
zend_result vector_calc_add(zend_object *self, zend_object *other, zval *return_value)
{
	if (vector_check_same_count(self, other, "add") == FAILURE) {
		return FAILURE;
	}

	zend_long count = vector_read_count(self);
	if (vector_create(return_value, count) == FAILURE) {
		return FAILURE;
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < count; i++) {
		double a, b;
		vector_read_element(self, i, &a);
		vector_read_element(other, i, &b);
		if (vector_write_result(result, i, a + b, "add the given Vectors") == FAILURE) {
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			return FAILURE;
		}
	}

	return SUCCESS;
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

	if (vector_calc_add(Z_OBJ_P(ZEND_THIS), Z_OBJ_P(other), return_value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ vector_calc_sub
 *
 * The computational core of sub(), shared with the `-` operator (vector_operators.c). Matches the
 * PHP package's Vector::sub().
 */
zend_result vector_calc_sub(zend_object *self, zend_object *other, zval *return_value)
{
	if (vector_check_same_count(self, other, "subtract") == FAILURE) {
		return FAILURE;
	}

	zend_long count = vector_read_count(self);
	if (vector_create(return_value, count) == FAILURE) {
		return FAILURE;
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < count; i++) {
		double a, b;
		vector_read_element(self, i, &a);
		vector_read_element(other, i, &b);
		if (vector_write_result(result, i, a - b, "subtract the given Vectors") == FAILURE) {
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			return FAILURE;
		}
	}

	return SUCCESS;
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

	if (vector_calc_sub(Z_OBJ_P(ZEND_THIS), Z_OBJ_P(other), return_value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ vector_calc_mul_scalar
 *
 * The computational core of mul()'s scalar branch, shared with the `*` operator
 * (vector_operators.c).
 */
zend_result vector_calc_mul_scalar(zend_object *self, double scalar, zval *return_value)
{
	if (math_floats_check_scalar(scalar) == FAILURE) {
		return FAILURE;
	}

	zend_string *scalar_str = math_floats_to_str(scalar);

	zend_long count = vector_read_count(self);
	if (vector_create(return_value, count) == FAILURE) {
		zend_string_release(scalar_str);
		return FAILURE;
	}
	zend_object *result = Z_OBJ_P(return_value);

	zend_string *operation = strpprintf(0, "multiply the given Vector by %s", ZSTR_VAL(scalar_str));
	zend_string_release(scalar_str);
	for (zend_long i = 0; i < count; i++) {
		double element;
		vector_read_element(self, i, &element);
		if (vector_write_result(result, i, element * scalar, ZSTR_VAL(operation)) == FAILURE) {
			zend_string_release(operation);
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			return FAILURE;
		}
	}
	zend_string_release(operation);

	return SUCCESS;
}
/* }}} */

/* {{{ vector_calc_mul_matrix
 *
 * The computational core of mul()'s Matrix branch, shared with the `*` operator
 * (vector_operators.c): converts this Vector to a row Matrix, multiplies by $other, and extracts
 * the (only) resulting row as a Vector - `$this->toRowMatrix()->mul($other)->getRow(0)`.
 */
zend_result vector_calc_mul_matrix(zend_object *self, zend_object *other, zval *return_value)
{
	/* Checked here, rather than left to matrix_calc_mul_matrix(), so the message names this Vector's count, as the
	 * package's does. */
	zend_long count = vector_read_count(self);
	zend_long row_count = matrix_read_row_count(other);
	if (count != row_count) {
		zend_string *msg = strpprintf(
			0, "Invalid Matrix row count: " ZEND_LONG_FMT ". Must equal this Vector's count: " ZEND_LONG_FMT ".",
			row_count, count);
		zend_throw_exception(spl_ce_LengthException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	zval row_matrix;
	if (vector_to_row_matrix(self, &row_matrix) == FAILURE) {
		return FAILURE;
	}

	zval product;
	zend_result mul_result = matrix_calc_mul_matrix(Z_OBJ(row_matrix), other, &product);
	zval_ptr_dtor(&row_matrix);
	if (mul_result == FAILURE) {
		math_rethrow_overflow("multiply the given Vector and Matrix");
		return FAILURE;
	}

	zend_object *row0 = matrix_read_row(Z_OBJ(product), 0);
	if (row0 == NULL) {
		zval_ptr_dtor(&product);
		return FAILURE;
	}
	zend_object *cloned = row0->handlers->clone_obj(row0);
	zval_ptr_dtor(&product);

	ZVAL_OBJ(return_value, cloned);
	return SUCCESS;
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::mul(float|Matrix $multiplier): Vector
 *
 * Matches the PHP package's Vector::mul(): scalar branch scales every element; Matrix branch
 * converts this Vector to a row Matrix, multiplies by $multiplier, and extracts the (only)
 * resulting row as a Vector.
 */
PHP_METHOD(OceanMoon_Math_Vector, mul)
{
	zval *multiplier;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(multiplier)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *self = Z_OBJ_P(ZEND_THIS);

	if (Z_TYPE_P(multiplier) == IS_DOUBLE || Z_TYPE_P(multiplier) == IS_LONG) {
		double scalar = Z_TYPE_P(multiplier) == IS_DOUBLE ? Z_DVAL_P(multiplier) : (double) Z_LVAL_P(multiplier);
		if (vector_calc_mul_scalar(self, scalar, return_value) == FAILURE) {
			RETURN_THROWS();
		}
		return;
	}

	if (Z_TYPE_P(multiplier) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(multiplier), matrix_ce_Matrix)) {
		zend_argument_type_error(
			1, "must be of type OceanMoon\\Math\\Matrix|float, %s given", zend_zval_value_name(multiplier)
		);
		RETURN_THROWS();
	}

	if (vector_calc_mul_matrix(self, Z_OBJ_P(multiplier), return_value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ vector_calc_div
 *
 * The computational core of div(), shared with normalize() (vector_linear_algebra.c). Matches
 * the PHP package's Vector::div().
 */
zend_result vector_calc_div(zend_object *obj, double scalar, zval *return_value)
{
	/* Dividing by ±INF would otherwise silently give a zero vector. */
	if (math_floats_check_scalar(scalar) == FAILURE) {
		return FAILURE;
	}

	if (scalar == 0.0) {
		zend_throw_exception(math_ce_ArithmeticException, "Cannot divide by zero.", 0);
		return FAILURE;
	}

	zend_long count = vector_read_count(obj);
	if (vector_create(return_value, count) == FAILURE) {
		return FAILURE;
	}
	zend_object *result = Z_OBJ_P(return_value);

	zend_string *scalar_str = math_floats_to_str(scalar);
	zend_string *operation = strpprintf(0, "divide the given Vector by %s", ZSTR_VAL(scalar_str));
	zend_string_release(scalar_str);
	for (zend_long i = 0; i < count; i++) {
		double element;
		vector_read_element(obj, i, &element);
		if (vector_write_result(result, i, element / scalar, ZSTR_VAL(operation)) == FAILURE) {
			zend_string_release(operation);
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			return FAILURE;
		}
	}
	zend_string_release(operation);

	return SUCCESS;
}
/* }}} */

/* {{{ vector_calc_scalar_div
 *
 * The computational core of the `/` operator's `float / Vector` form (vector_operators.c) -
 * Vector has no named method for this direction, only the operator (`Vector / float` is div(),
 * above). Element-wise: divides $scalar by each element in turn, equivalent to `$scalar *
 * $vector->hadamardInv()`. Throws ArithmeticException for a zero element, matching hadamardInv(). Throws
 * DomainException for a non-finite scalar, like the other scalar forms.
 */
zend_result vector_calc_scalar_div(double scalar, zend_object *obj, zval *return_value)
{
	if (math_floats_check_scalar(scalar) == FAILURE) {
		return FAILURE;
	}

	zend_long count = vector_read_count(obj);
	if (vector_create(return_value, count) == FAILURE) {
		return FAILURE;
	}
	zend_object *result = Z_OBJ_P(return_value);

	zend_string *scalar_str = math_floats_to_str(scalar);
	zend_string *operation = strpprintf(0, "divide %s by the given Vector", ZSTR_VAL(scalar_str));
	zend_string_release(scalar_str);

	for (zend_long i = 0; i < count; i++) {
		double element;
		vector_read_element(obj, i, &element);

		if (element == 0.0) {
			zend_string *msg = strpprintf(0, "Cannot divide by zero at index " ZEND_LONG_FMT ".", i);
			zend_throw_exception(math_ce_ArithmeticException, ZSTR_VAL(msg), 0);
			zend_string_release(msg);
			zend_string_release(operation);
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			return FAILURE;
		}

		if (vector_write_result(result, i, scalar / element, ZSTR_VAL(operation)) == FAILURE) {
			zend_string_release(operation);
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			return FAILURE;
		}
	}
	zend_string_release(operation);

	return SUCCESS;
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::div(float $scalar): Vector
 *
 * Matches the PHP package's Vector::div(): divides every element by the scalar.
 */
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

/* {{{ vector_calc_hadamard_mul
 *
 * The core of hadamardMul(), shared with the `*` operator's `Vector * Vector` form
 * (vector_operators.c). Matches the PHP package's Vector::hadamardMul(): element-wise product.
 */
zend_result vector_calc_hadamard_mul(zend_object *self, zend_object *other, zval *return_value)
{
	if (vector_check_same_count(self, other, "compute Hadamard product with") == FAILURE) {
		return FAILURE;
	}

	zend_long count = vector_read_count(self);
	if (vector_create(return_value, count) == FAILURE) {
		return FAILURE;
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < count; i++) {
		double a, b;
		vector_read_element(self, i, &a);
		vector_read_element(other, i, &b);
		if (vector_write_result(result, i, a * b, "compute Hadamard product of the given Vectors") == FAILURE) {
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			return FAILURE;
		}
	}

	return SUCCESS;
}
/* }}} */

/* {{ OceanMoon\Math\Vector::hadamardMul(Vector $other): Vector
 *
 * Matches the PHP package's Vector::hadamardMul().
 */
PHP_METHOD(OceanMoon_Math_Vector, hadamardMul)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(other, vector_ce_Vector)
	ZEND_PARSE_PARAMETERS_END();

	if (vector_calc_hadamard_mul(Z_OBJ_P(ZEND_THIS), Z_OBJ_P(other), return_value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ vector_calc_hadamard_div
 *
 * The core of hadamardDiv(), shared with the `/` operator's `Vector / Vector` form
 * (vector_operators.c). Matches the PHP package's Vector::hadamardDiv(): element-wise quotient.
 */
zend_result vector_calc_hadamard_div(zend_object *self, zend_object *other, zval *return_value)
{
	if (vector_check_same_count(self, other, "compute Hadamard quotient with") == FAILURE) {
		return FAILURE;
	}

	zend_long count = vector_read_count(self);
	if (vector_create(return_value, count) == FAILURE) {
		return FAILURE;
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < count; i++) {
		double a, b;
		vector_read_element(self, i, &a);
		vector_read_element(other, i, &b);

		if (b == 0.0) {
			zend_string *msg = strpprintf(0, "Cannot divide by zero at index " ZEND_LONG_FMT ".", i);
			zend_throw_exception(math_ce_ArithmeticException, ZSTR_VAL(msg), 0);
			zend_string_release(msg);
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			return FAILURE;
		}

		if (vector_write_result(result, i, a / b, "compute Hadamard quotient of the given Vectors") == FAILURE) {
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			return FAILURE;
		}
	}

	return SUCCESS;
}
/* }}} */

/* {{ OceanMoon\Math\Vector::hadamardDiv(Vector $other): Vector
 *
 * Matches the PHP package's Vector::hadamardDiv().
 */
PHP_METHOD(OceanMoon_Math_Vector, hadamardDiv)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(other, vector_ce_Vector)
	ZEND_PARSE_PARAMETERS_END();

	if (vector_calc_hadamard_div(Z_OBJ_P(ZEND_THIS), Z_OBJ_P(other), return_value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */
