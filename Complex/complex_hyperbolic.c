/*
 * complex_hyperbolic.c
 *
 * Hyperbolic and inverse hyperbolic methods for OceanMoon\Math\Complex: sinh(), cosh(), tanh(),
 * asinh(), acosh(), atanh(). Mirrors the "Hyperbolic methods"/"Inverse hyperbolic methods" regions
 * of the PHP package's Complex class. See complex_trigonometric.c's header comment for why the
 * inverse methods here work their add/sub/mul/div/sqrt/ln chains in plain doubles rather than
 * promoting each intermediate value to a real Complex object.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <math.h>

#include "php.h"
#include "Zend/zend_exceptions.h"
#include "complex_internal.h"
#include "complex_arginfo.h"
#include "exceptions.h"

/* {{{ c_sqrt
 *
 * Principal square root of a plain (real, imaginary) pair. Duplicated (rather than shared) from
 * complex_trigonometric.c's static of the same name -- both are small, file-local pure functions,
 * not worth promoting to complex_internal.h for two call sites.
 */
static void c_sqrt(double re, double im, double *out_re, double *out_im)
{
	double magnitude = (im == 0.0) ? fabs(re) : hypot(re, im);
	double phase = (im == 0.0) ? (re < 0 ? M_PI : 0.0) : atan2(im, re);

	double sqrt_mag = sqrt(magnitude);
	double half_phase = phase / 2.0;
	*out_re = sqrt_mag * cos(half_phase);
	*out_im = sqrt_mag * sin(half_phase);
}
/* }}} */

/* {{{ c_ln
 *
 * Natural logarithm of a plain (real, imaginary) pair. Returns FAILURE (with ArithmeticException
 * already thrown) for zero, matching OceanMoon_Math_Complex::ln().
 */
static zend_result c_ln(double re, double im, double *out_re, double *out_im)
{
	if (re == 0.0 && im == 0.0) {
		zend_throw_exception(math_ce_ArithmeticException, "Cannot compute logarithm of zero.", 0);
		return FAILURE;
	}

	double magnitude = (im == 0.0) ? fabs(re) : hypot(re, im);
	double phase = (im == 0.0) ? (re < 0 ? M_PI : 0.0) : atan2(im, re);

	*out_re = log(magnitude);
	*out_im = phase;
	return SUCCESS;
}
/* }}} */

/* {{{ c_div
 *
 * Complex division of two plain (real, imaginary) pairs. Returns FAILURE (with ArithmeticException
 * already thrown) for division by zero, matching OceanMoon_Math_Complex::div().
 */
static zend_result c_div(double a_re, double a_im, double b_re, double b_im, double *out_re, double *out_im)
{
	if (b_re == 0.0 && b_im == 0.0) {
		zend_throw_exception(math_ce_ArithmeticException, "Cannot divide by zero.", 0);
		return FAILURE;
	}

	double f = (b_re * b_re) + (b_im * b_im);
	*out_re = (a_re * b_re + a_im * b_im) / f;
	*out_im = (a_im * b_re - a_re * b_im) / f;
	return SUCCESS;
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::sinh(): Complex
 *
 * Matches the PHP package's Complex::sinh(): sinh(z) = sinh(x)cos(y) + i·cosh(x)sin(y), z = x + iy.
 */
PHP_METHOD(OceanMoon_Math_Complex, sinh)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double x, y;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &x, &y);

	if (complex_create(return_value, sinh(x) * cos(y), cosh(x) * sin(y)) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::cosh(): Complex
 *
 * Matches the PHP package's Complex::cosh(): cosh(z) = cosh(x)cos(y) + i·sinh(x)sin(y), z = x + iy.
 */
PHP_METHOD(OceanMoon_Math_Complex, cosh)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double x, y;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &x, &y);

	if (complex_create(return_value, cosh(x) * cos(y), sinh(x) * sin(y)) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::tanh(): Complex
 *
 * Matches the PHP package's Complex::tanh(): tanh(z) = sinh(z) / cosh(z).
 */
PHP_METHOD(OceanMoon_Math_Complex, tanh)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double x, y;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &x, &y);

	double sinh_re = sinh(x) * cos(y), sinh_im = cosh(x) * sin(y);
	double cosh_re = cosh(x) * cos(y), cosh_im = sinh(x) * sin(y);

	double out_re, out_im;
	if (c_div(sinh_re, sinh_im, cosh_re, cosh_im, &out_re, &out_im) == FAILURE) {
		RETURN_THROWS();
	}

	if (complex_create(return_value, out_re, out_im) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::asinh(): Complex
 *
 * Matches the PHP package's Complex::asinh(): asinh(z) = ln(z + √(z²+1)).
 */
PHP_METHOD(OceanMoon_Math_Complex, asinh)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double x, y;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &x, &y);

	/* z^2 + 1. */
	double z2_plus_1_re = (x * x - y * y) + 1.0;
	double z2_plus_1_im = 2 * x * y;

	double sqrt_re, sqrt_im;
	c_sqrt(z2_plus_1_re, z2_plus_1_im, &sqrt_re, &sqrt_im);

	double sum_re = x + sqrt_re, sum_im = y + sqrt_im;

	double out_re, out_im;
	if (c_ln(sum_re, sum_im, &out_re, &out_im) == FAILURE) {
		RETURN_THROWS();
	}

	if (complex_create(return_value, out_re, out_im) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::acosh(): Complex
 *
 * Matches the PHP package's Complex::acosh(): acosh(z) = ln(z + √(z²-1)).
 */
PHP_METHOD(OceanMoon_Math_Complex, acosh)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double x, y;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &x, &y);

	/* z^2 - 1. */
	double z2_minus_1_re = (x * x - y * y) - 1.0;
	double z2_minus_1_im = 2 * x * y;

	double sqrt_re, sqrt_im;
	c_sqrt(z2_minus_1_re, z2_minus_1_im, &sqrt_re, &sqrt_im);

	double sum_re = x + sqrt_re, sum_im = y + sqrt_im;

	double out_re, out_im;
	if (c_ln(sum_re, sum_im, &out_re, &out_im) == FAILURE) {
		RETURN_THROWS();
	}

	if (complex_create(return_value, out_re, out_im) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::atanh(): Complex
 *
 * Matches the PHP package's Complex::atanh(): atanh(z) = (1/2)·ln((1+z)/(1-z)).
 */
PHP_METHOD(OceanMoon_Math_Complex, atanh)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double x, y;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &x, &y);

	/* 1 + z, 1 - z. */
	double one_plus_z_re = 1 + x, one_plus_z_im = y;
	double one_minus_z_re = 1 - x, one_minus_z_im = -y;

	double div_re, div_im;
	if (c_div(one_plus_z_re, one_plus_z_im, one_minus_z_re, one_minus_z_im, &div_re, &div_im) == FAILURE) {
		RETURN_THROWS();
	}

	double ln_re, ln_im;
	if (c_ln(div_re, div_im, &ln_re, &ln_im) == FAILURE) {
		RETURN_THROWS();
	}

	if (complex_create(return_value, 0.5 * ln_re, 0.5 * ln_im) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */
