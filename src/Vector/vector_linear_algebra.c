/*
 * vector_linear_algebra.c
 *
 * Linear algebra methods for OceanMoon\Math\Vector: dot(), cross(), outer(), normalize(). Mirrors
 * the "Linear algebra methods" region of the PHP package's Vector class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <math.h>

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "vector_internal.h"
#include "../../oceanmoon_math_arginfo.h"
#include "../Matrix/matrix_internal.h"
#include "exceptions.h"
#include "floats.h"

/* {{{ vector_scale
 *
 * Ports the PHP package's Vector::powerOfTwoScale() for a Vector's elements: the power of 2 that scales the
 * largest absolute element into the range [1, 2), or 0.0 for a zero or empty vector.
 */
static double vector_scale(zend_object *obj)
{
	zend_long count = vector_read_count(obj);
	double max = 0.0;

	for (zend_long i = 0; i < count; i++) {
		double element;
		vector_read_element(obj, i, &element);
		max = fmax(max, fabs(element));
	}

	return math_floats_power_of_two_scale(max);
}
/* }}} */

/* {{{ vector_compute_magnitude
 *
 * Matches the PHP package's Vector::$magnitude hook: scales the elements by a power of 2 so their squares can't
 * overflow or underflow, takes sqrt(sum of squares), then undoes the scaling. Backs the `magnitude` computed
 * property (vector_properties.c), and is also called directly (bypassing property read dispatch) by normalize()
 * below.
 */
double vector_compute_magnitude(zend_object *obj)
{
	/* Handle a zero or empty vector. */
	double scale = vector_scale(obj);
	if (scale == 0.0) {
		return 0.0;
	}

	zend_long count = vector_read_count(obj);
	double sum = 0.0;
	for (zend_long i = 0; i < count; i++) {
		double element;
		/* Bounds are trivially satisfied (0..count-1), so this never fails. */
		vector_read_element(obj, i, &element);
		double scaled = element / scale;
		sum += scaled * scaled;
	}

	return scale * sqrt(sum);
}
/* }}} */

/* {{{ vector_calc_cross
 *
 * Ports the PHP package's Vector::calcCross(): the cross product of two 3-element arrays, without overflow
 * checks, so the result may include ±INF or NAN.
 */
static void vector_calc_cross(const double a[3], const double b[3], double out[3])
{
	out[0] = a[1] * b[2] - a[2] * b[1];
	out[1] = a[2] * b[0] - a[0] * b[2];
	out[2] = a[0] * b[1] - a[1] * b[0];
}
/* }}} */

/* {{{ vector_all_finite
 *
 * Ports the PHP package's Vector::allFinite() for a 3-element array.
 */
static bool vector_all_finite(const double values[3])
{
	return zend_finite(values[0]) && zend_finite(values[1]) && zend_finite(values[2]);
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::dot(Vector $other): float
 *
 * Matches the PHP package's Vector::dot().
 */
PHP_METHOD(OceanMoon_Math_Vector, dot)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(other, vector_ce_Vector)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_object *other_obj = Z_OBJ_P(other);

	zend_long count = vector_read_count(self);
	zend_long other_count = vector_read_count(other_obj);
	if (count != other_count) {
		zend_string *msg = strpprintf(
			0, "Cannot compute dot product with Vector of incorrect count: " ZEND_LONG_FMT ". Expected "
			ZEND_LONG_FMT ".", other_count, count
		);
		zend_throw_exception(spl_ce_LengthException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		RETURN_THROWS();
	}

	double sum = 0.0;
	for (zend_long i = 0; i < count; i++) {
		double a, b;
		vector_read_element(self, i, &a);
		vector_read_element(other_obj, i, &b);
		sum += a * b;
	}

	RETURN_DOUBLE(sum);
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::cross(Vector $other): Vector
 *
 * Matches the PHP package's Vector::cross(): both vectors must be count 3.
 */
PHP_METHOD(OceanMoon_Math_Vector, cross)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(other, vector_ce_Vector)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_object *other_obj = Z_OBJ_P(other);

	zend_long self_count = vector_read_count(self);
	if (self_count != 3) {
		zend_string *msg = strpprintf(
			0, "Cannot compute cross product with first Vector count: " ZEND_LONG_FMT ". Must be 3.", self_count
		);
		zend_throw_exception(spl_ce_LengthException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		RETURN_THROWS();
	}

	zend_long other_count = vector_read_count(other_obj);
	if (other_count != 3) {
		zend_string *msg = strpprintf(
			0, "Cannot compute cross product with second Vector count: " ZEND_LONG_FMT ". Must be 3.", other_count
		);
		zend_throw_exception(spl_ce_LengthException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		RETURN_THROWS();
	}

	/* Calculate the cross product directly, which is exact for ordinary values. */
	double a[3], b[3], cross[3];
	for (zend_long i = 0; i < 3; i++) {
		vector_read_element(self, i, &a[i]);
		vector_read_element(other_obj, i, &b[i]);
	}
	vector_calc_cross(a, b, cross);

	/* If an intermediate product overflowed, scale each vector so its largest element is in the range [1, 2), and
	 * try again. Scaling by a power of 2 is exact. Neither vector can be zero here, or every product would be 0. */
	if (!vector_all_finite(cross)) {
		double scale_a = vector_scale(self);
		double scale_b = vector_scale(other_obj);
		double scaled_a[3], scaled_b[3];
		for (int i = 0; i < 3; i++) {
			scaled_a[i] = a[i] / scale_a;
			scaled_b[i] = b[i] / scale_b;
		}
		vector_calc_cross(scaled_a, scaled_b, cross);

		/* Undo the scaling, applying the smaller scale first, so only a genuine overflow gives INF. */
		double min_scale = fmin(scale_a, scale_b);
		double max_scale = fmax(scale_a, scale_b);
		for (int i = 0; i < 3; i++) {
			cross[i] = cross[i] * min_scale * max_scale;
		}
		if (!vector_all_finite(cross)) {
			zval self_str, other_str;
			ZVAL_STR(&self_str, zval_get_string(ZEND_THIS));
			ZVAL_STR(&other_str, zval_get_string(other));
			zend_string *msg = strpprintf(
				0, "Cannot compute cross product of %s and %s. The result would overflow.",
				Z_STRVAL(self_str), Z_STRVAL(other_str)
			);
			zval_ptr_dtor(&self_str);
			zval_ptr_dtor(&other_str);
			zend_throw_exception(spl_ce_OverflowException, ZSTR_VAL(msg), 0);
			zend_string_release(msg);
			RETURN_THROWS();
		}
	}

	if (vector_create(return_value, 3) == FAILURE) {
		RETURN_THROWS();
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < 3; i++) {
		if (vector_write_element(result, i, cross[i]) == FAILURE) {
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			RETURN_THROWS();
		}
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::outer(Vector $other): Matrix
 *
 * Matches the PHP package's Vector::outer(): `$this->toColumnMatrix()->mul($other->toRowMatrix())`.
 */
PHP_METHOD(OceanMoon_Math_Vector, outer)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(other, vector_ce_Vector)
	ZEND_PARSE_PARAMETERS_END();

	zval column_matrix;
	if (vector_to_column_matrix(Z_OBJ_P(ZEND_THIS), &column_matrix) == FAILURE) {
		RETURN_THROWS();
	}

	zval row_matrix;
	if (vector_to_row_matrix(Z_OBJ_P(other), &row_matrix) == FAILURE) {
		zval_ptr_dtor(&column_matrix);
		RETURN_THROWS();
	}

	zend_result result = matrix_calc_mul_matrix(Z_OBJ(column_matrix), Z_OBJ(row_matrix), return_value);
	zval_ptr_dtor(&column_matrix);
	zval_ptr_dtor(&row_matrix);
	if (result == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::normalize(): Vector
 *
 * Matches the PHP package's Vector::normalize(): scales the vector so its largest element is in the range [1, 2),
 * then divides by the magnitude, which can't overflow. Both divisions go through the same vector_calc_div() core
 * div() itself uses (vector_arithmetic.c).
 */
PHP_METHOD(OceanMoon_Math_Vector, normalize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	/* Check for a zero (or empty) vector. */
	zend_object *self = Z_OBJ_P(ZEND_THIS);
	double scale = vector_scale(self);
	if (scale == 0.0) {
		zend_throw_exception(math_ce_ArithmeticException, "Cannot normalize zero Vector.", 0);
		RETURN_THROWS();
	}

	zval scaled;
	if (vector_calc_div(self, scale, &scaled) == FAILURE) {
		RETURN_THROWS();
	}

	zend_result result = vector_calc_div(Z_OBJ(scaled), vector_compute_magnitude(Z_OBJ(scaled)), return_value);
	zval_ptr_dtor(&scaled);
	if (result == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */
