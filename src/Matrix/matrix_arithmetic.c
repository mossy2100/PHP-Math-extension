/*
 * matrix_arithmetic.c
 *
 * Unary and binary arithmetic methods for OceanMoon\Math\Matrix: neg(), reciprocal(), inv(),
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

/* {{{ matrix_check_scalar
 *
 * Throws DomainException "Invalid scalar: <value>. Must be finite." for a non-finite scalar argument, matching the
 * PHP package's Matrix::mul() and div().
 */
static zend_result matrix_check_scalar(double scalar)
{
	if (zend_finite(scalar)) {
		return SUCCESS;
	}
	zend_string *scalar_str = math_floats_to_str(scalar);
	zend_string *msg = strpprintf(0, "Invalid scalar: %s. Must be finite.", ZSTR_VAL(scalar_str));
	zend_string_release(scalar_str);
	zend_throw_exception(spl_ce_DomainException, ZSTR_VAL(msg), 0);
	zend_string_release(msg);
	return FAILURE;
}
/* }}} */

/* {{{ matrix_scalar_operation
 *
 * Formats an overflow message's operation for a scalar operand, e.g. "multiply Matrix by 1.0E+200". The format
 * has one %s, for the scalar. The caller releases the result.
 */
static zend_string *matrix_scalar_operation(const char *format, double scalar)
{
	zend_string *scalar_str = math_floats_to_str(scalar);
	zend_string *operation = strpprintf(0, format, ZSTR_VAL(scalar_str));
	zend_string_release(scalar_str);
	return operation;
}
/* }}} */

/* {{{ matrix_to_data_array
 *
 * Builds a list<list<float>> zval from a Matrix object, matching toArray()'s output - used by
 * inv()/det() as the input to matrix_calc_det().
 */
static void matrix_to_data_array(zend_object *obj, zval *out)
{
	zend_long row_count = matrix_read_row_count(obj);
	zend_long column_count = matrix_read_column_count(obj);

	array_init_size(out, (uint32_t) row_count);
	for (zend_long i = 0; i < row_count; i++) {
		zval row;
		array_init_size(&row, (uint32_t) column_count);
		for (zend_long j = 0; j < column_count; j++) {
			double value;
			matrix_read_element(obj, i, j, &value);
			zval v;
			ZVAL_DOUBLE(&v, value);
			zend_hash_next_index_insert(Z_ARRVAL(row), &v);
		}
		zend_hash_next_index_insert(Z_ARRVAL_P(out), &row);
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::neg(): Matrix
 *
 * Matches the PHP package's Matrix::neg(): `$this->mul(-1)`.
 */
PHP_METHOD(OceanMoon_Math_Matrix, neg)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);

	if (matrix_create(return_value, row_count, column_count) == FAILURE) {
		RETURN_THROWS();
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < row_count; i++) {
		for (zend_long j = 0; j < column_count; j++) {
			double value;
			matrix_read_element(self, i, j, &value);
			if (matrix_write_element(result, i, j, -value) == FAILURE) {
				zval_ptr_dtor(return_value);
				ZVAL_UNDEF(return_value);
				RETURN_THROWS();
			}
		}
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::reciprocal(): Matrix
 *
 * Matches the PHP package's Matrix::reciprocal(): element-wise reciprocal, not the matrix inverse.
 */
PHP_METHOD(OceanMoon_Math_Matrix, reciprocal)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);

	if (matrix_create(return_value, row_count, column_count) == FAILURE) {
		RETURN_THROWS();
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < row_count; i++) {
		for (zend_long j = 0; j < column_count; j++) {
			double value;
			matrix_read_element(self, i, j, &value);
			if (value == 0.0) {
				zend_string *msg = strpprintf(
					0, "Cannot compute reciprocal of zero at row " ZEND_LONG_FMT ", column " ZEND_LONG_FMT ".",
					i, j
				);
				zend_throw_exception(math_ce_ArithmeticException, ZSTR_VAL(msg), 0);
				zend_string_release(msg);
				zval_ptr_dtor(return_value);
				ZVAL_UNDEF(return_value);
				RETURN_THROWS();
			}
			if (matrix_write_result(result, i, j, 1.0 / value, "compute reciprocal of Matrix") == FAILURE) {
				zval_ptr_dtor(return_value);
				ZVAL_UNDEF(return_value);
				RETURN_THROWS();
			}
		}
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::inv(): Matrix
 *
 * Matches the PHP package's Matrix::inv(): cofactor expansion via the adjugate matrix. The actual
 * logic lives in matrix_calc_inv() (this file), shared with pow()'s negative-exponent branch
 * (matrix_power.c), matching the "shared calc core" pattern used elsewhere in this extension.
 */
PHP_METHOD(OceanMoon_Math_Matrix, inv)
{
	ZEND_PARSE_PARAMETERS_NONE();

	if (matrix_calc_inv(Z_OBJ_P(ZEND_THIS), return_value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ matrix_calc_inv
 *
 * The shared core behind inv(): cofactor expansion via the adjugate matrix. Used directly by
 * pow()'s negative-exponent branch (matrix_power.c) as well as by inv() itself.
 *
 * Matches the PHP package's Matrix::inv(). It first checks the matrix isn't singular, by LU decomposition. A matrix
 * larger than 3x3 is then inverted from that decomposition. One up to 3x3 is inverted with the adjugate formula,
 * which gives exact results where the arithmetic allows. That tries without scaling first, so ordinary results are
 * unchanged, and a matrix whose elements span a wide range isn't harmed by scaling its smallest
 * elements down to 0. If the determinant underflows to 0 or overflows, as it can for a matrix of
 * uniformly tiny or huge elements, it scales the matrix so the largest element is in the range
 * [1, 2) and tries again. Scaling by a power of 2 is exact. inv(A) = inv(A / s) / s.
 */
zend_result matrix_calc_inv(zend_object *self, zval *return_value)
{
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);

	if (row_count != column_count) {
		zend_throw_exception(spl_ce_DomainException, "Cannot invert non-square Matrix.", 0);
		return FAILURE;
	}

	zend_long n = row_count;

	/* Check the matrix isn't singular. */
	zval data_array;
	matrix_to_data_array(self, &data_array);
	matrix_lu lu;
	matrix_lu_decompose(&data_array, &lu);
	if (lu.singular) {
		matrix_lu_free(&lu);
		zval_ptr_dtor(&data_array);
		zend_throw_exception(math_ce_ArithmeticException, "Cannot invert singular Matrix.", 0);
		return FAILURE;
	}

	/* Invert a matrix larger than 3x3 by LU decomposition. */
	if (n > 3) {
		zend_result result = matrix_inv_from_lu(&lu, return_value);
		matrix_lu_free(&lu);
		zval_ptr_dtor(&data_array);
		return result;
	}
	matrix_lu_free(&lu);

	/* Invert a matrix up to 3x3 with the adjugate formula. Try without scaling first. */
	double scale = 1.0;
	zend_object *scaled = self;
	zval scaled_zv;
	ZVAL_UNDEF(&scaled_zv);

	double det = matrix_calc_det(&data_array);
	zval_ptr_dtor(&data_array);

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
			scale = math_floats_power_of_two_scale(max);
			if (matrix_calc_div_scalar(self, scale, &scaled_zv) == FAILURE) {
				return FAILURE;
			}
			scaled = Z_OBJ(scaled_zv);
			matrix_to_data_array(scaled, &data_array);
			det = matrix_calc_det(&data_array);
			zval_ptr_dtor(&data_array);
		}
	}

	/* The determinant is finite here: either it was already, or the matrix was scaled so every element is less
	 * than 2. It's non-zero unless it underflowed even after scaling, which the singularity check above makes
	 * unlikely. */
	if (det == 0.0) {
		zval_ptr_dtor(&scaled_zv);
		zend_throw_exception(math_ce_ArithmeticException, "Cannot invert singular Matrix.", 0);
		return FAILURE;
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
			/* Normalize, as negating a zero cofactor gives -0.0. */
			double element = math_floats_normalize_zero(cofactor / det / scale);
			if (matrix_write_result(adjugate, j, i, element, "invert Matrix") == FAILURE) {
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

	if (matrix_create(return_value, row_count, column_count) == FAILURE) {
		return FAILURE;
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < row_count; i++) {
		for (zend_long j = 0; j < column_count; j++) {
			double a, b;
			matrix_read_element(self, i, j, &a);
			matrix_read_element(other, i, j, &b);
			if (matrix_write_result(result, i, j, a + b, "add Matrices") == FAILURE) {
				zval_ptr_dtor(return_value);
				ZVAL_UNDEF(return_value);
				return FAILURE;
			}
		}
	}

	return SUCCESS;
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

	if (matrix_create(return_value, row_count, column_count) == FAILURE) {
		return FAILURE;
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < row_count; i++) {
		for (zend_long j = 0; j < column_count; j++) {
			double a, b;
			matrix_read_element(self, i, j, &a);
			matrix_read_element(other, i, j, &b);
			if (matrix_write_result(result, i, j, a - b, "subtract Matrices") == FAILURE) {
				zval_ptr_dtor(return_value);
				ZVAL_UNDEF(return_value);
				return FAILURE;
			}
		}
	}

	return SUCCESS;
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
	if (matrix_check_scalar(scalar) == FAILURE) {
		return FAILURE;
	}

	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);

	if (matrix_create(return_value, row_count, column_count) == FAILURE) {
		return FAILURE;
	}
	zend_object *result = Z_OBJ_P(return_value);

	zend_string *operation = matrix_scalar_operation("multiply Matrix by %s", scalar);
	for (zend_long i = 0; i < row_count; i++) {
		for (zend_long j = 0; j < column_count; j++) {
			double value;
			matrix_read_element(self, i, j, &value);
			if (matrix_write_result(result, i, j, value * scalar, ZSTR_VAL(operation)) == FAILURE) {
				zend_string_release(operation);
				zval_ptr_dtor(return_value);
				ZVAL_UNDEF(return_value);
				return FAILURE;
			}
		}
	}
	zend_string_release(operation);

	return SUCCESS;
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

	if (vector_create(return_value, row_count) == FAILURE) {
		return FAILURE;
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < row_count; i++) {
		double sum = 0.0;
		for (zend_long k = 0; k < column_count; k++) {
			double a, b;
			matrix_read_element(self, i, k, &a);
			vector_read_element(vector, k, &b);
			sum += a * b;
		}
		if (vector_write_result(result, i, sum, "multiply Matrix by Vector") == FAILURE) {
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			return FAILURE;
		}
	}

	return SUCCESS;
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
		zend_argument_type_error(1, "must be of type float|Vector|Matrix, %s given", zend_zval_type_name(other));
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

	if (matrix_create(return_value, row_count, other_column_count) == FAILURE) {
		return FAILURE;
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < row_count; i++) {
		for (zend_long j = 0; j < other_column_count; j++) {
			double sum = 0.0;
			for (zend_long k = 0; k < column_count; k++) {
				double a, b;
				matrix_read_element(self, i, k, &a);
				matrix_read_element(other, k, j, &b);
				sum += a * b;
			}
			if (matrix_write_result(result, i, j, sum, "multiply Matrices") == FAILURE) {
				zval_ptr_dtor(return_value);
				ZVAL_UNDEF(return_value);
				return FAILURE;
			}
		}
	}

	return SUCCESS;
}
/* }}} */

/* {{{ matrix_calc_div_scalar
 *
 * The computational core of div(), shared with the `/` operator (matrix_operators.c).
 */
zend_result matrix_calc_div_scalar(zend_object *self, double scalar, zval *return_value)
{
	/* Dividing by ±INF would otherwise silently give a zero matrix. */
	if (matrix_check_scalar(scalar) == FAILURE) {
		return FAILURE;
	}

	if (scalar == 0.0) {
		zend_throw_exception(math_ce_ArithmeticException, "Cannot divide by zero.", 0);
		return FAILURE;
	}

	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);

	if (matrix_create(return_value, row_count, column_count) == FAILURE) {
		return FAILURE;
	}
	zend_object *result = Z_OBJ_P(return_value);

	zend_string *operation = matrix_scalar_operation("divide Matrix by %s", scalar);
	for (zend_long i = 0; i < row_count; i++) {
		for (zend_long j = 0; j < column_count; j++) {
			double value;
			matrix_read_element(self, i, j, &value);
			if (matrix_write_result(result, i, j, value / scalar, ZSTR_VAL(operation)) == FAILURE) {
				zend_string_release(operation);
				zval_ptr_dtor(return_value);
				ZVAL_UNDEF(return_value);
				return FAILURE;
			}
		}
	}
	zend_string_release(operation);

	return SUCCESS;
}
/* }}} */

/* {{{ matrix_calc_scalar_div
 *
 * The computational core of the `/` operator's `float / Matrix` form (matrix_operators.c) -
 * Matrix has no named method for this direction, only the operator (`Matrix / float` is div(),
 * above). Element-wise: divides $scalar by each element in turn, equivalent to `$scalar *
 * $matrix->reciprocal()`. Throws ArithmeticException for a zero element, matching reciprocal().
 */
zend_result matrix_calc_scalar_div(double scalar, zend_object *self, zval *return_value)
{
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);

	if (matrix_create(return_value, row_count, column_count) == FAILURE) {
		return FAILURE;
	}
	zend_object *result = Z_OBJ_P(return_value);

	zend_string *operation = matrix_scalar_operation("divide %s by Matrix", scalar);
	for (zend_long i = 0; i < row_count; i++) {
		for (zend_long j = 0; j < column_count; j++) {
			double value;
			matrix_read_element(self, i, j, &value);
			if (value == 0.0) {
				zend_string *msg = strpprintf(
					0, "Cannot divide by zero at row " ZEND_LONG_FMT ", column " ZEND_LONG_FMT ".", i, j
				);
				zend_throw_exception(math_ce_ArithmeticException, ZSTR_VAL(msg), 0);
				zend_string_release(msg);
				zend_string_release(operation);
				zval_ptr_dtor(return_value);
				ZVAL_UNDEF(return_value);
				return FAILURE;
			}
			if (matrix_write_result(result, i, j, scalar / value, ZSTR_VAL(operation)) == FAILURE) {
				zend_string_release(operation);
				zval_ptr_dtor(return_value);
				ZVAL_UNDEF(return_value);
				return FAILURE;
			}
		}
	}
	zend_string_release(operation);

	return SUCCESS;
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

	if (matrix_create(return_value, row_count, column_count) == FAILURE) {
		RETURN_THROWS();
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < row_count; i++) {
		for (zend_long j = 0; j < column_count; j++) {
			double a, b;
			matrix_read_element(self, i, j, &a);
			matrix_read_element(other_obj, i, j, &b);
			if (matrix_write_result(result, i, j, a * b, "multiply Matrices element-wise") == FAILURE) {
				zval_ptr_dtor(return_value);
				ZVAL_UNDEF(return_value);
				RETURN_THROWS();
			}
		}
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

	if (matrix_create(return_value, row_count, column_count) == FAILURE) {
		RETURN_THROWS();
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < row_count; i++) {
		for (zend_long j = 0; j < column_count; j++) {
			double a, divisor;
			matrix_read_element(self, i, j, &a);
			matrix_read_element(other_obj, i, j, &divisor);
			if (divisor == 0.0) {
				zend_string *msg = strpprintf(
					0, "Cannot divide by zero at row " ZEND_LONG_FMT ", column " ZEND_LONG_FMT ".", i, j
				);
				zend_throw_exception(math_ce_ArithmeticException, ZSTR_VAL(msg), 0);
				zend_string_release(msg);
				zval_ptr_dtor(return_value);
				ZVAL_UNDEF(return_value);
				RETURN_THROWS();
			}
			if (matrix_write_result(result, i, j, a / divisor, "divide Matrices element-wise") == FAILURE) {
				zval_ptr_dtor(return_value);
				ZVAL_UNDEF(return_value);
				RETURN_THROWS();
			}
		}
	}
}
/* }}} */
