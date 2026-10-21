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

/* Element-wise operations, for vector_calc_elementwise(). The ones with a scalar operand come last. */
typedef enum {
	VECTOR_NEG,
	VECTOR_RECIPROCAL,
	VECTOR_ADD,
	VECTOR_SUB,
	VECTOR_HADAMARD_MUL,
	VECTOR_HADAMARD_DIV,
	VECTOR_MUL_SCALAR,
	VECTOR_DIV_SCALAR,
	VECTOR_SCALAR_DIV
} vector_elementwise_op;

/* {{{ vector_calc_elementwise
 *
 * The shared core of the element-wise methods. Applies op to each element of self, with the same element of other or
 * with scalar, and stores the results in a new Vector in return_value. The caller has already checked other's count
 * and the scalar. Like the package, it fails at the first element that has a zero divisor (ArithmeticException) or a
 * non-finite result (OverflowException "Cannot <operation>. The result would overflow."). For an operation with a
 * scalar operand, operation has one %s, for the scalar, and the message is only formatted if it's thrown.
 */
static zend_result vector_calc_elementwise(
	vector_elementwise_op op, zend_object *self, zend_object *other, double scalar, const char *operation,
	zval *return_value
)
{
	zend_long count = vector_read_count(self);

	double *values = vector_read_all(self);
	if (values == NULL) {
		return FAILURE;
	}
	double *other_values = NULL;
	if (other != NULL) {
		other_values = vector_read_all(other);
		if (other_values == NULL) {
			efree(values);
			return FAILURE;
		}
	}

	zend_result status = SUCCESS;
	for (zend_long i = 0; i < count; i++) {
		double divisor = 1.0;
		double result;
		switch (op) {
			case VECTOR_NEG:
				result = -values[i];
				break;
			case VECTOR_RECIPROCAL:
				divisor = values[i];
				result = 1.0 / divisor;
				break;
			case VECTOR_ADD:
				result = values[i] + other_values[i];
				break;
			case VECTOR_SUB:
				result = values[i] - other_values[i];
				break;
			case VECTOR_HADAMARD_MUL:
				result = values[i] * other_values[i];
				break;
			case VECTOR_HADAMARD_DIV:
				divisor = other_values[i];
				result = values[i] / divisor;
				break;
			case VECTOR_MUL_SCALAR:
				result = values[i] * scalar;
				break;
			case VECTOR_DIV_SCALAR:
				result = values[i] / scalar;
				break;
			case VECTOR_SCALAR_DIV:
				divisor = values[i];
				result = scalar / divisor;
				break;
			EMPTY_SWITCH_DEFAULT_CASE()
		}

		if (divisor == 0.0) {
			zend_string *msg = strpprintf(
				0, "Cannot %s at index " ZEND_LONG_FMT ".",
				op == VECTOR_RECIPROCAL ? "compute reciprocal of zero" : "divide by zero", i
			);
			zend_throw_exception(math_ce_ArithmeticException, ZSTR_VAL(msg), 0);
			zend_string_release(msg);
			status = FAILURE;
			break;
		}
		if (!zend_finite(result)) {
			if (op >= VECTOR_MUL_SCALAR) {
				math_throw_scalar_overflow(operation, scalar);
			} else {
				math_throw_overflow(operation);
			}
			status = FAILURE;
			break;
		}
		values[i] = result;
	}

	if (status == SUCCESS) {
		vector_create_from(return_value, count, values);
	}
	efree(values);
	if (other_values != NULL) {
		efree(other_values);
	}
	return status;
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::neg(): Vector
 *
 * Matches the PHP package's Vector::neg(): mul(-1).
 */
PHP_METHOD(OceanMoon_Math_Vector, neg)
{
	ZEND_PARSE_PARAMETERS_NONE();

	if (vector_calc_elementwise(VECTOR_NEG, Z_OBJ_P(ZEND_THIS), NULL, 0.0, "negate the given Vector", return_value)
		== FAILURE) {
		RETURN_THROWS();
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

	if (vector_calc_elementwise(
		VECTOR_RECIPROCAL, Z_OBJ_P(ZEND_THIS), NULL, 0.0, "compute reciprocal of the given Vector", return_value
	) == FAILURE) {
		RETURN_THROWS();
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

	return vector_calc_elementwise(VECTOR_ADD, self, other, 0.0, "add the given Vectors", return_value);
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

	return vector_calc_elementwise(VECTOR_SUB, self, other, 0.0, "subtract the given Vectors", return_value);
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

	return vector_calc_elementwise(
		VECTOR_MUL_SCALAR, self, NULL, scalar, "multiply the given Vector by %s", return_value
	);
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

	return vector_calc_elementwise(
		VECTOR_DIV_SCALAR, obj, NULL, scalar, "divide the given Vector by %s", return_value
	);
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

	return vector_calc_elementwise(
		VECTOR_SCALAR_DIV, obj, NULL, scalar, "divide %s by the given Vector", return_value
	);
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

	return vector_calc_elementwise(
		VECTOR_HADAMARD_MUL, self, other, 0.0, "compute Hadamard product of the given Vectors", return_value
	);
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

	return vector_calc_elementwise(
		VECTOR_HADAMARD_DIV, self, other, 0.0, "compute Hadamard quotient of the given Vectors", return_value
	);
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
