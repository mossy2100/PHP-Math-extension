/*
 * matrix_arithmetic.c
 *
 * Unary and binary arithmetic methods for OceanMoon\Math\Matrix: neg(), hadamardInv(), inv(),
 * add(), sub(), mul(), div(), hadamardMul(), hadamardDiv(). Mirrors the "Unary arithmetic methods"
 * and "Binary arithmetic methods" regions of the PHP package's Matrix class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <math.h>

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "matrix_internal.h"
#include "../../oceanmoon_math_arginfo.h"
#include "exceptions.h"
#include "floats.h"

/* Element-wise operations, for matrix_calc_elementwise(). */
typedef enum {
	MATRIX_NEG,
	MATRIX_RECIPROCAL,
	MATRIX_ADD,
	MATRIX_SUB,
	MATRIX_HADAMARD_MUL,
	MATRIX_HADAMARD_DIV,
	MATRIX_MUL_SCALAR,
	MATRIX_DIV_SCALAR,
	MATRIX_SCALAR_DIV
} matrix_elementwise_op;

/* {{{ matrix_calc_elementwise
 *
 * The shared core of the element-wise methods. Applies op to each element of self, with the same element of other or
 * with scalar, and stores the results in a new Matrix in return_value. The caller has already checked other's
 * dimensions and the scalar. Like the package, it works in row-major order, and fails at the first element that has a
 * zero divisor (ArithmeticException) or a non-finite result (OverflowException "Cannot <operation>. The result would
 * overflow.").
 */
static zend_result matrix_calc_elementwise(
	matrix_elementwise_op op, zend_object *self, zend_object *other, double scalar, const char *operation,
	zval *return_value
)
{
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);

	double *values = matrix_read_all(self);
	if (values == NULL) {
		return FAILURE;
	}
	double *other_values = NULL;
	if (other != NULL) {
		other_values = matrix_read_all(other);
		if (other_values == NULL) {
			efree(values);
			return FAILURE;
		}
	}

	zend_result status = SUCCESS;
	zend_long count = row_count * column_count;
	for (zend_long k = 0; k < count; k++) {
		double divisor = 1.0;
		double result;
		switch (op) {
			case MATRIX_NEG:
				result = -values[k];
				break;
			case MATRIX_RECIPROCAL:
				divisor = values[k];
				result = 1.0 / divisor;
				break;
			case MATRIX_ADD:
				result = values[k] + other_values[k];
				break;
			case MATRIX_SUB:
				result = values[k] - other_values[k];
				break;
			case MATRIX_HADAMARD_MUL:
				result = values[k] * other_values[k];
				break;
			case MATRIX_HADAMARD_DIV:
				divisor = other_values[k];
				result = values[k] / divisor;
				break;
			case MATRIX_MUL_SCALAR:
				result = values[k] * scalar;
				break;
			case MATRIX_DIV_SCALAR:
				result = values[k] / scalar;
				break;
			case MATRIX_SCALAR_DIV:
				divisor = values[k];
				result = scalar / divisor;
				break;
			EMPTY_SWITCH_DEFAULT_CASE()
		}

		if (divisor == 0.0) {
			zend_string *msg = strpprintf(
				0, "Cannot %s at row " ZEND_LONG_FMT ", column " ZEND_LONG_FMT ".",
				op == MATRIX_RECIPROCAL ? "compute reciprocal of zero" : "divide by zero",
				k / column_count, k % column_count
			);
			zend_throw_exception(math_ce_ArithmeticException, ZSTR_VAL(msg), 0);
			zend_string_release(msg);
			status = FAILURE;
			break;
		}
		if (!zend_finite(result)) {
			math_throw_overflow(operation);
			status = FAILURE;
			break;
		}
		values[k] = result;
	}

	if (status == SUCCESS) {
		matrix_create_from(return_value, row_count, column_count, values);
	}
	efree(values);
	if (other_values != NULL) {
		efree(other_values);
	}
	return status;
}
/* }}} */

/* {{{ matrix_calc_scalar_elementwise
 *
 * matrix_calc_elementwise() for an operation with a scalar operand. format is the overflow message's operation, with
 * one %s for the scalar.
 */
static zend_result matrix_calc_scalar_elementwise(
	matrix_elementwise_op op, zend_object *self, double scalar, const char *format, zval *return_value
)
{
	zend_string *scalar_str = math_floats_to_str(scalar);
	zend_string *operation = strpprintf(0, format, ZSTR_VAL(scalar_str));
	zend_string_release(scalar_str);
	zend_result result = matrix_calc_elementwise(op, self, NULL, scalar, ZSTR_VAL(operation), return_value);
	zend_string_release(operation);
	return result;
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::neg(): Matrix
 *
 * Matches the PHP package's Matrix::neg(): `$this->mul(-1)`.
 */
PHP_METHOD(OceanMoon_Math_Matrix, neg)
{
	ZEND_PARSE_PARAMETERS_NONE();

	if (matrix_calc_elementwise(MATRIX_NEG, Z_OBJ_P(ZEND_THIS), NULL, 0.0, "negate the given Matrix", return_value)
		== FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::hadamardInv(): Matrix
 *
 * Matches the PHP package's Matrix::hadamardInv(): element-wise reciprocal, not the matrix inverse.
 */
PHP_METHOD(OceanMoon_Math_Matrix, hadamardInv)
{
	ZEND_PARSE_PARAMETERS_NONE();

	if (matrix_calc_elementwise(
		MATRIX_RECIPROCAL, Z_OBJ_P(ZEND_THIS), NULL, 0.0, "compute reciprocal of the given Matrix", return_value
	) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::inv(): Matrix
 *
 * Matches the PHP package's Matrix::inv(). The actual logic lives in matrix_calc_inv() (this file),
 * shared with pow()'s negative-exponent branch (matrix_power.c), matching the "shared calc core"
 * pattern used elsewhere in this extension.
 */
PHP_METHOD(OceanMoon_Math_Matrix, inv)
{
	ZEND_PARSE_PARAMETERS_NONE();

	if (matrix_calc_inv(Z_OBJ_P(ZEND_THIS), return_value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ matrix_invert
 *
 * Inverts a square matrix that has passed the pivot test in matrix_lu_decompose(), mirroring Matrix::calcInv(). A
 * matrix larger than 3x3 is inverted from the LU decomposition. One up to 3x3 is inverted with the adjugate formula,
 * which gives exact results where the arithmetic allows. That tries without scaling first, so ordinary results are
 * unchanged, and a matrix whose elements span a wide range isn't harmed by scaling its smallest elements down to 0. If
 * the determinant underflows to 0 or overflows, as it can for a matrix of uniformly tiny or huge elements, it scales
 * the matrix so the largest element is in the range [1, 2) and tries again. Scaling by a power of 2 is exact.
 * inv(A) = inv(A / s) / s. The caller still owns data_array and lu.
 */
static zend_result matrix_invert(zend_object *self, zval *data_array, const matrix_lu *lu, zval *return_value)
{
	zend_long n = lu->n;

	/* Invert a matrix larger than 3x3 by LU decomposition. */
	if (n > 3) {
		return matrix_inv_from_lu(lu, return_value);
	}

	/* Invert a matrix up to 3x3 with the adjugate formula. Try without scaling first. */
	double scale = 1.0;
	zend_object *scaled = self;
	zval scaled_zv;
	ZVAL_UNDEF(&scaled_zv);

	double det = matrix_calc_det(data_array);

	/* If the determinant underflowed to 0 or overflowed, scale so the largest element is in the
	 * range [1, 2), and try again. */
	if (det == 0.0 || !zend_finite(det)) {
		double max = 0.0;
		for (zend_long i = 0; i < n; i++) {
			for (zend_long j = 0; j < n; j++) {
				double value;
				matrix_read_element(self, i, j, &value);
				max = fmax(max, fabs(value));
			}
		}
		if (max != 0.0) {
			scale = math_floats_power_of_2_scale(max);
			if (matrix_calc_div_scalar(self, scale, &scaled_zv) == FAILURE) {
				return FAILURE;
			}
			scaled = Z_OBJ(scaled_zv);
			zval scaled_data;
			if (matrix_to_data_array(scaled, &scaled_data) == FAILURE) {
				zval_ptr_dtor(&scaled_zv);
				return FAILURE;
			}
			det = matrix_calc_det(&scaled_data);
			zval_ptr_dtor(&scaled_data);
		}
	}

	/* The determinant is finite here: either it was already, or the matrix was scaled so every element is less
	 * than 2. It can still be 0 if it underflowed even after scaling, which happens when the elements span a wide
	 * range, e.g. diag(1, 1e-200, 1e-200). The singularity check has passed, so use the LU decomposition instead. */
	if (det == 0.0) {
		zval_ptr_dtor(&scaled_zv);
		return matrix_inv_from_lu(lu, return_value);
	}

	if (matrix_create(return_value, n, n) == FAILURE) {
		zval_ptr_dtor(&scaled_zv);
		return FAILURE;
	}
	zend_object *adjugate = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < n; i++) {
		for (zend_long j = 0; j < n; j++) {
			zval minor;
			matrix_get_minor(scaled, i, j, &minor);
			double minor_det = matrix_calc_det(&minor);
			zval_ptr_dtor(&minor);

			double cofactor = ((i + j) % 2 == 0 ? 1.0 : -1.0) * minor_det;
			/* Note: transposed. */
			double element = cofactor / det / scale;
			if (matrix_write_result(adjugate, j, i, element, "invert the given Matrix") == FAILURE) {
				zval_ptr_dtor(&scaled_zv);
				zval_ptr_dtor(return_value);
				ZVAL_UNDEF(return_value);
				return FAILURE;
			}
		}
	}

	zval_ptr_dtor(&scaled_zv);
	return SUCCESS;
}
/* }}} */

/* {{{ matrix_calc_inv
 *
 * The shared core behind inv(), used directly by pow()'s negative-exponent branch (matrix_power.c) as well as by inv()
 * itself. Matches the PHP package's Matrix::inv(). It first checks the matrix isn't singular, by LU decomposition,
 * then inverts it with matrix_invert(). Rounding during elimination can leave a singular matrix with pivots that pass
 * that test, so the inverse is then checked with matrix_is_ill_conditioned() too.
 */
zend_result matrix_calc_inv(zend_object *self, zval *return_value)
{
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);

	if (row_count != column_count) {
		zend_throw_exception(spl_ce_DomainException, "Cannot invert non-square Matrix.", 0);
		return FAILURE;
	}

	/* Check the matrix isn't singular. */
	zval data_array;
	if (matrix_to_data_array(self, &data_array) == FAILURE) {
		return FAILURE;
	}
	matrix_lu lu;
	matrix_lu_decompose(&data_array, &lu);
	if (lu.singular) {
		matrix_lu_free(&lu);
		zval_ptr_dtor(&data_array);
		zend_throw_exception(math_ce_ArithmeticException, "Cannot invert singular Matrix.", 0);
		return FAILURE;
	}

	/* Invert it, then check it's not too ill-conditioned for the inverse to mean anything. */
	zend_result result = matrix_invert(self, &data_array, &lu, return_value);
	zval_ptr_dtor(&data_array);
	if (result == SUCCESS && matrix_is_ill_conditioned(self, Z_OBJ_P(return_value), &lu)) {
		zval_ptr_dtor(return_value);
		ZVAL_UNDEF(return_value);
		zend_throw_exception(math_ce_ArithmeticException, "Cannot invert singular Matrix.", 0);
		result = FAILURE;
	}
	matrix_lu_free(&lu);

	return result;
}
/* }}} */

/* {{{ matrix_calc_add
 *
 * The computational core of add(), shared with the `+` operator (matrix_operators.c). Matches the
 * PHP package's Matrix::add().
 */
zend_result matrix_calc_add(zend_object *self, zend_object *other, zval *return_value)
{
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);
	zend_long other_row_count = matrix_read_row_count(other);
	zend_long other_column_count = matrix_read_column_count(other);

	if (row_count != other_row_count || column_count != other_column_count) {
		zend_string *msg = strpprintf(
			0, "Cannot add Matrix of incorrect dimensions: " ZEND_LONG_FMT "x" ZEND_LONG_FMT ". "
			"Expected " ZEND_LONG_FMT "x" ZEND_LONG_FMT ".",
			other_row_count, other_column_count, row_count, column_count
		);
		zend_throw_exception(spl_ce_LengthException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	return matrix_calc_elementwise(MATRIX_ADD, self, other, 0.0, "add the given Matrices", return_value);
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::add(Matrix $other): Matrix
 *
 * Matches the PHP package's Matrix::add().
 */
PHP_METHOD(OceanMoon_Math_Matrix, add)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(other, matrix_ce_Matrix)
	ZEND_PARSE_PARAMETERS_END();

	if (matrix_calc_add(Z_OBJ_P(ZEND_THIS), Z_OBJ_P(other), return_value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ matrix_calc_sub
 *
 * The computational core of sub(), shared with the `-` operator (matrix_operators.c). Matches the
 * PHP package's Matrix::sub().
 */
zend_result matrix_calc_sub(zend_object *self, zend_object *other, zval *return_value)
{
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);
	zend_long other_row_count = matrix_read_row_count(other);
	zend_long other_column_count = matrix_read_column_count(other);

	if (row_count != other_row_count || column_count != other_column_count) {
		zend_string *msg = strpprintf(
			0, "Cannot subtract Matrix of incorrect dimensions: " ZEND_LONG_FMT "x" ZEND_LONG_FMT ". "
			"Expected " ZEND_LONG_FMT "x" ZEND_LONG_FMT ".",
			other_row_count, other_column_count, row_count, column_count
		);
		zend_throw_exception(spl_ce_LengthException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	return matrix_calc_elementwise(MATRIX_SUB, self, other, 0.0, "subtract the given Matrices", return_value);
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::sub(Matrix $other): Matrix
 *
 * Matches the PHP package's Matrix::sub().
 */
PHP_METHOD(OceanMoon_Math_Matrix, sub)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(other, matrix_ce_Matrix)
	ZEND_PARSE_PARAMETERS_END();

	if (matrix_calc_sub(Z_OBJ_P(ZEND_THIS), Z_OBJ_P(other), return_value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ matrix_calc_mul_scalar
 *
 * The computational core of mul()'s scalar branch, shared with the `*` operator
 * (matrix_operators.c).
 */
zend_result matrix_calc_mul_scalar(zend_object *self, double scalar, zval *return_value)
{
	if (math_floats_check_scalar(scalar) == FAILURE) {
		return FAILURE;
	}

	return matrix_calc_scalar_elementwise(
		MATRIX_MUL_SCALAR, self, scalar, "multiply the given Matrix by %s", return_value
	);
}
/* }}} */

/* {{{ matrix_calc_mul_vector
 *
 * The shared core behind mul()'s Vector branch (Ax, treating the vector as a column vector), also
 * used by the `*` operator's `Matrix * Vector` form (matrix_operators.c). Matches the PHP package's
 * Matrix::mul(): element i of the result is the dot product of this Matrix's row i with $vector.
 */
zend_result matrix_calc_mul_vector(zend_object *self, zend_object *vector, zval *return_value)
{
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);
	zend_long vector_count = vector_read_count(vector);

	if (column_count != vector_count) {
		zend_string *msg = strpprintf(
			0, "Invalid Vector count: " ZEND_LONG_FMT ". Must equal this Matrix's column count: "
			ZEND_LONG_FMT ".", vector_count, column_count
		);
		zend_throw_exception(spl_ce_LengthException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	double *values = matrix_read_all(self);
	if (values == NULL) {
		return FAILURE;
	}
	double *vector_values = vector_read_all(vector);
	if (vector_values == NULL) {
		efree(values);
		return FAILURE;
	}
	double *results = safe_emalloc((size_t) (row_count > 0 ? row_count : 1), sizeof(double), 0);

	zend_result status = SUCCESS;
	for (zend_long i = 0; i < row_count; i++) {
		double sum = 0.0;
		for (zend_long k = 0; k < column_count; k++) {
			sum += values[i * column_count + k] * vector_values[k];
		}
		if (!zend_finite(sum)) {
			math_throw_overflow("multiply the given Matrix and Vector");
			status = FAILURE;
			break;
		}
		results[i] = sum;
	}

	if (status == SUCCESS) {
		vector_create_from(return_value, row_count, results);
	}
	efree(values);
	efree(vector_values);
	efree(results);
	return status;
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::mul(float|Vector|Matrix $other): Vector|Matrix
 *
 * Matches the PHP package's Matrix::mul(): scalar branch scales every element; Vector branch gives
 * Ax as a Vector; Matrix branch does real matrix multiplication.
 */
PHP_METHOD(OceanMoon_Math_Matrix, mul)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(other)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *self = Z_OBJ_P(ZEND_THIS);

	if (Z_TYPE_P(other) == IS_DOUBLE || Z_TYPE_P(other) == IS_LONG) {
		double scalar = Z_TYPE_P(other) == IS_DOUBLE ? Z_DVAL_P(other) : (double) Z_LVAL_P(other);
		if (matrix_calc_mul_scalar(self, scalar, return_value) == FAILURE) {
			RETURN_THROWS();
		}
		return;
	}

	if (Z_TYPE_P(other) == IS_OBJECT && instanceof_function(Z_OBJCE_P(other), vector_ce_Vector)) {
		if (matrix_calc_mul_vector(self, Z_OBJ_P(other), return_value) == FAILURE) {
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

	if (matrix_calc_mul_matrix(self, Z_OBJ_P(other), return_value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ matrix_calc_mul_matrix
 *
 * The shared core behind mul()'s Matrix branch: real matrix multiplication. Used directly by
 * sqr()/pow() (matrix_power.c) as well as by mul() itself.
 */
zend_result matrix_calc_mul_matrix(zend_object *self, zend_object *other, zval *return_value)
{
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);
	zend_long other_row_count = matrix_read_row_count(other);
	zend_long other_column_count = matrix_read_column_count(other);

	if (column_count != other_row_count) {
		zend_string *msg = strpprintf(
			0, "Invalid Matrix row count: " ZEND_LONG_FMT ". Must equal this Matrix's column count: "
			ZEND_LONG_FMT ".", other_row_count, column_count
		);
		zend_throw_exception(spl_ce_LengthException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	double *values = matrix_read_all(self);
	if (values == NULL) {
		return FAILURE;
	}
	double *other_values = matrix_read_all(other);
	if (other_values == NULL) {
		efree(values);
		return FAILURE;
	}
	double *results = safe_emalloc(
		(size_t) (row_count > 0 ? row_count : 1),
		(size_t) (other_column_count > 0 ? other_column_count : 1) * sizeof(double), 0
	);

	zend_result status = SUCCESS;
	for (zend_long i = 0; i < row_count && status == SUCCESS; i++) {
		for (zend_long j = 0; j < other_column_count; j++) {
			double sum = 0.0;
			for (zend_long k = 0; k < column_count; k++) {
				sum += values[i * column_count + k] * other_values[k * other_column_count + j];
			}
			if (!zend_finite(sum)) {
				math_throw_overflow("multiply the given Matrices");
				status = FAILURE;
				break;
			}
			results[i * other_column_count + j] = sum;
		}
	}

	if (status == SUCCESS) {
		matrix_create_from(return_value, row_count, other_column_count, results);
	}
	efree(values);
	efree(other_values);
	efree(results);
	return status;
}
/* }}} */

/* {{{ matrix_calc_div_scalar
 *
 * The computational core of div(), shared with the `/` operator (matrix_operators.c).
 */
zend_result matrix_calc_div_scalar(zend_object *self, double scalar, zval *return_value)
{
	/* Dividing by ±INF would otherwise silently give a zero matrix. */
	if (math_floats_check_scalar(scalar) == FAILURE) {
		return FAILURE;
	}

	if (scalar == 0.0) {
		zend_throw_exception(math_ce_ArithmeticException, "Cannot divide by zero.", 0);
		return FAILURE;
	}

	return matrix_calc_scalar_elementwise(
		MATRIX_DIV_SCALAR, self, scalar, "divide the given Matrix by %s", return_value
	);
}
/* }}} */

/* {{{ matrix_calc_scalar_div
 *
 * The computational core of the `/` operator's `float / Matrix` form (matrix_operators.c) -
 * Matrix has no named method for this direction, only the operator (`Matrix / float` is div(),
 * above). Element-wise: divides $scalar by each element in turn, equivalent to `$scalar *
 * $matrix->hadamardInv()`. Throws ArithmeticException for a zero element, matching hadamardInv(). Throws
 * DomainException for a non-finite scalar, like the other scalar forms.
 */
zend_result matrix_calc_scalar_div(double scalar, zend_object *self, zval *return_value)
{
	if (math_floats_check_scalar(scalar) == FAILURE) {
		return FAILURE;
	}

	return matrix_calc_scalar_elementwise(
		MATRIX_SCALAR_DIV, self, scalar, "divide %s by the given Matrix", return_value
	);
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::div(float $scalar): Matrix
 *
 * Matches the PHP package's Matrix::div().
 */
PHP_METHOD(OceanMoon_Math_Matrix, div)
{
	double scalar;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(scalar)
	ZEND_PARSE_PARAMETERS_END();

	if (matrix_calc_div_scalar(Z_OBJ_P(ZEND_THIS), scalar, return_value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::hadamardMul(Matrix $other): Matrix
 *
 * Matches the PHP package's Matrix::hadamardMul().
 */
PHP_METHOD(OceanMoon_Math_Matrix, hadamardMul)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(other, matrix_ce_Matrix)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_object *other_obj = Z_OBJ_P(other);

	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);
	zend_long other_row_count = matrix_read_row_count(other_obj);
	zend_long other_column_count = matrix_read_column_count(other_obj);

	if (row_count != other_row_count || column_count != other_column_count) {
		zend_string *msg = strpprintf(
			0, "Cannot compute Hadamard product with Matrix of incorrect dimensions: "
			ZEND_LONG_FMT "x" ZEND_LONG_FMT ". Expected " ZEND_LONG_FMT "x" ZEND_LONG_FMT ".",
			other_row_count, other_column_count, row_count, column_count
		);
		zend_throw_exception(spl_ce_LengthException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		RETURN_THROWS();
	}

	if (matrix_calc_elementwise(
		MATRIX_HADAMARD_MUL, self, other_obj, 0.0, "compute Hadamard product of the given Matrices", return_value
	) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::hadamardDiv(Matrix $other): Matrix
 *
 * Matches the PHP package's Matrix::hadamardDiv().
 */
PHP_METHOD(OceanMoon_Math_Matrix, hadamardDiv)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(other, matrix_ce_Matrix)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_object *other_obj = Z_OBJ_P(other);

	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);
	zend_long other_row_count = matrix_read_row_count(other_obj);
	zend_long other_column_count = matrix_read_column_count(other_obj);

	if (row_count != other_row_count || column_count != other_column_count) {
		zend_string *msg = strpprintf(
			0, "Cannot compute Hadamard quotient with Matrix of incorrect dimensions: "
			ZEND_LONG_FMT "x" ZEND_LONG_FMT ". Expected " ZEND_LONG_FMT "x" ZEND_LONG_FMT ".",
			other_row_count, other_column_count, row_count, column_count
		);
		zend_throw_exception(spl_ce_LengthException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		RETURN_THROWS();
	}

	if (matrix_calc_elementwise(
		MATRIX_HADAMARD_DIV, self, other_obj, 0.0, "compute Hadamard quotient of the given Matrices", return_value
	) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */
