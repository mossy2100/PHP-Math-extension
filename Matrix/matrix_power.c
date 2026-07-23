/*
 * matrix_power.c
 *
 * Power methods for OceanMoon\Math\Matrix: pow(), sqr(). Mirrors the "Power methods" region of the
 * PHP package's Matrix class, composing matrix_calc_mul_matrix()/matrix_calc_inv() (the shared
 * cores behind mul()/inv() in matrix_arithmetic.c) directly rather than round-tripping through the
 * PHP_METHOD entry points.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "matrix_internal.h"
#include "matrix_arginfo.h"

/* {{{ matrix_identity
 *
 * Builds an identity Matrix of the given size into *out. Mirrors Matrix::identity(), used
 * internally by matrix_pow_positive() (no bounds/exception handling needed here since $size is
 * always this->rowCount, already known valid).
 */
static zend_result matrix_identity(zend_long size, zval *out)
{
	if (matrix_create(out, size, size) == FAILURE) {
		return FAILURE;
	}
	zend_object *obj = Z_OBJ_P(out);
	for (zend_long i = 0; i < size; i++) {
		if (matrix_write_element(obj, i, i, 1.0) == FAILURE) {
			zval_ptr_dtor(out);
			ZVAL_UNDEF(out);
			return FAILURE;
		}
	}
	return SUCCESS;
}
/* }}} */

/* {{{ matrix_pow_positive
 *
 * Exponentiation by squaring for a positive $exponent (>= 1), used by pow() for its main case and
 * for the PHP_INT_MIN special case (as pow(PHP_INT_MAX)). Assumes $self is already known square;
 * callers are responsible for that check.
 */
static zend_result matrix_pow_positive(zend_object *self, zend_long exponent, zval *out)
{
	zend_long row_count = matrix_read_row_count(self);

	if (matrix_identity(row_count, out) == FAILURE) {
		return FAILURE;
	}

	zend_object *base_obj = self->handlers->clone_obj(self);
	zval base;
	ZVAL_OBJ(&base, base_obj);

	while (exponent > 0) {
		if (exponent % 2 == 1) {
			zval next_result;
			if (matrix_calc_mul_matrix(Z_OBJ_P(out), Z_OBJ(base), &next_result) == FAILURE) {
				zval_ptr_dtor(out);
				ZVAL_UNDEF(out);
				zval_ptr_dtor(&base);
				return FAILURE;
			}
			zval_ptr_dtor(out);
			ZVAL_COPY_VALUE(out, &next_result);
		}

		exponent = exponent / 2;
		if (exponent == 0) {
			break;
		}

		zval next_base;
		if (matrix_calc_mul_matrix(Z_OBJ(base), Z_OBJ(base), &next_base) == FAILURE) {
			zval_ptr_dtor(out);
			ZVAL_UNDEF(out);
			zval_ptr_dtor(&base);
			return FAILURE;
		}
		zval_ptr_dtor(&base);
		ZVAL_COPY_VALUE(&base, &next_base);
	}

	zval_ptr_dtor(&base);
	return SUCCESS;
}
/* }}} */

/* {{{ matrix_calc_pow
 *
 * The computational core of pow(), shared with the `**` operator (matrix_operators.c): identity
 * for 0, clone for 1, exponentiation by squaring for larger positive exponents, and
 * inv()->pow(-exponent) for negative ones (with a special case for PHP_INT_MIN, whose negation
 * overflows: pow(PHP_INT_MAX)->mul(this)->inv()).
 */
zend_result matrix_calc_pow(zend_object *self, zend_long exponent, zval *return_value)
{
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);

	if (row_count != column_count) {
		zend_throw_exception(spl_ce_DomainException, "Cannot raise non-square Matrix to a power.", 0);
		return FAILURE;
	}

	/* Handle power of 0. */
	if (exponent == 0) {
		return matrix_identity(row_count, return_value);
	}

	/* Handle power of 1. */
	if (exponent == 1) {
		zend_object *cloned = self->handlers->clone_obj(self);
		ZVAL_OBJ(return_value, cloned);
		return SUCCESS;
	}

	/* Handle exponent = PHP_INT_MIN: pow(PHP_INT_MAX)->mul(this)->inv(). */
	if (exponent == ZEND_LONG_MIN) {
		zval max_pow;
		if (matrix_pow_positive(self, ZEND_LONG_MAX, &max_pow) == FAILURE) {
			return FAILURE;
		}

		zval mul_result;
		if (matrix_calc_mul_matrix(Z_OBJ(max_pow), self, &mul_result) == FAILURE) {
			zval_ptr_dtor(&max_pow);
			return FAILURE;
		}
		zval_ptr_dtor(&max_pow);

		zend_result result = matrix_calc_inv(Z_OBJ(mul_result), return_value);
		zval_ptr_dtor(&mul_result);
		return result;
	}

	/* Handle negative powers. */
	if (exponent < 0) {
		zval inv_result;
		if (matrix_calc_inv(self, &inv_result) == FAILURE) {
			return FAILURE;
		}

		zend_result result = matrix_pow_positive(Z_OBJ(inv_result), -exponent, return_value);
		zval_ptr_dtor(&inv_result);
		return result;
	}

	/* Handle positive powers greater than 1. */
	return matrix_pow_positive(self, exponent, return_value);
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::pow(int $exponent): Matrix
 *
 * Matches the PHP package's Matrix::pow().
 */
PHP_METHOD(OceanMoon_Math_Matrix, pow)
{
	zend_long exponent;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(exponent)
	ZEND_PARSE_PARAMETERS_END();

	if (matrix_calc_pow(Z_OBJ_P(ZEND_THIS), exponent, return_value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::sqr(): Matrix
 *
 * Matches the PHP package's Matrix::sqr(): `$this->mul($this)`, via matrix_calc_mul_matrix()
 * directly.
 */
PHP_METHOD(OceanMoon_Math_Matrix, sqr)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);

	if (row_count != column_count) {
		zend_throw_exception(spl_ce_DomainException, "Cannot square non-square Matrix.", 0);
		RETURN_THROWS();
	}

	if (matrix_calc_mul_matrix(self, self, return_value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */
