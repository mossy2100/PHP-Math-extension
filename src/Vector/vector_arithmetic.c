/*
 * vector_arithmetic.c
 *
 * Unary and binary arithmetic methods for OceanMoon\Math\Vector: neg(), reciprocal(), add(),
 * sub(), mul(), div(). Mirrors the "Unary arithmetic methods"/
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
 * Shared by add()/sub() and mul()/div()'s Vector branches: checks $other has the same count as self,
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

/* {{{ OceanMoon\Math\Vector::reciprocal(): Vector
 *
 * Matches the PHP package's Vector::reciprocal(): element-wise reciprocal.
 */
PHP_METHOD(OceanMoon_Math_Vector, reciprocal)
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
	zend_string *scalar_str = math_floats_to_str(scalar);

	if (!zend_finite(scalar)) {
		zend_string *msg = strpprintf(0, "Invalid scalar: %s. Must be finite.", ZSTR_VAL(scalar_str));
		zend_string_release(scalar_str);
		zend_throw_exception(spl_ce_DomainException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

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
	zend_object *cloned = row0->handlers->clone_obj(row0);
	zval_ptr_dtor(&product);

	ZVAL_OBJ(return_value, cloned);
	return SUCCESS;
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::mul(float|Vector|Matrix $other): Vector
 *
 * Matches the PHP package's Vector::mul(): scalar branch scales every element; Vector branch is
 * element-wise (Hadamard); Matrix branch converts this Vector to a row Matrix, multiplies by
 * $other, and extracts the (only) resulting row as a Vector.
 */
PHP_METHOD(OceanMoon_Math_Vector, mul)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(other)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *self = Z_OBJ_P(ZEND_THIS);

	if (Z_TYPE_P(other) == IS_DOUBLE || Z_TYPE_P(other) == IS_LONG) {
		double scalar = Z_TYPE_P(other) == IS_DOUBLE ? Z_DVAL_P(other) : (double) Z_LVAL_P(other);
		if (vector_calc_mul_scalar(self, scalar, return_value) == FAILURE) {
			RETURN_THROWS();
		}
		return;
	}

	if (Z_TYPE_P(other) == IS_OBJECT && instanceof_function(Z_OBJCE_P(other), vector_ce_Vector)) {
		if (vector_calc_mul_vector(self, Z_OBJ_P(other), return_value) == FAILURE) {
			RETURN_THROWS();
		}
		return;
	}

	if (Z_TYPE_P(other) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(other), matrix_ce_Matrix)) {
		zend_argument_type_error(
			1, "must be of type OceanMoon\\Math\\Vector|OceanMoon\\Math\\Matrix|float, %s given", zend_zval_value_name(other)
		);
		RETURN_THROWS();
	}

	if (vector_calc_mul_matrix(self, Z_OBJ_P(other), return_value) == FAILURE) {
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
	if (!zend_finite(scalar)) {
		zend_string *scalar_str = math_floats_to_str(scalar);
		zend_string *msg = strpprintf(0, "Invalid scalar: %s. Must be finite.", ZSTR_VAL(scalar_str));
		zend_string_release(scalar_str);
		zend_throw_exception(spl_ce_DomainException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
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
 * $vector->reciprocal()`. Throws ArithmeticException for a zero element, matching reciprocal().
 */
zend_result vector_calc_scalar_div(double scalar, zend_object *obj, zval *return_value)
{
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

/* {{{ OceanMoon\Math\Vector::div(float|Vector $other): Vector
 *
 * Matches the PHP package's Vector::div(): Vector branch is element-wise (Hadamard); scalar branch
 * divides every element.
 */
PHP_METHOD(OceanMoon_Math_Vector, div)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(other)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *self = Z_OBJ_P(ZEND_THIS);

	if (Z_TYPE_P(other) == IS_OBJECT && instanceof_function(Z_OBJCE_P(other), vector_ce_Vector)) {
		if (vector_calc_div_vector(self, Z_OBJ_P(other), return_value) == FAILURE) {
			RETURN_THROWS();
		}
		return;
	}

	if (Z_TYPE_P(other) != IS_DOUBLE && Z_TYPE_P(other) != IS_LONG) {
		zend_argument_type_error(
			1, "must be of type OceanMoon\\Math\\Vector|float, %s given", zend_zval_value_name(other)
		);
		RETURN_THROWS();
	}

	double scalar = Z_TYPE_P(other) == IS_DOUBLE ? Z_DVAL_P(other) : (double) Z_LVAL_P(other);
	if (vector_calc_div(self, scalar, return_value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ vector_calc_mul_vector
 *
 * The core of mul()'s Vector branch, shared with the `*` operator's `Vector * Vector` form
 * (vector_operators.c). Matches the PHP package's Vector::mul(): element-wise (Hadamard) product.
 */
zend_result vector_calc_mul_vector(zend_object *self, zend_object *other, zval *return_value)
{
	if (vector_check_same_count(self, other, "multiply by") == FAILURE) {
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
		if (vector_write_result(result, i, a * b, "multiply the given Vectors") == FAILURE) {
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			return FAILURE;
		}
	}

	return SUCCESS;
}
/* }}} */

/* {{{ vector_calc_div_vector
 *
 * The core of div()'s Vector branch, shared with the `/` operator's `Vector / Vector` form
 * (vector_operators.c). Matches the PHP package's Vector::div(): element-wise (Hadamard) quotient.
 */
zend_result vector_calc_div_vector(zend_object *self, zend_object *other, zval *return_value)
{
	if (vector_check_same_count(self, other, "divide by") == FAILURE) {
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

		if (vector_write_result(result, i, a / b, "divide the given Vectors") == FAILURE) {
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			return FAILURE;
		}
	}

	return SUCCESS;
}
/* }}} */
