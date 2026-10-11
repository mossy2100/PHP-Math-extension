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
#include "../../oceanmoon_math_arginfo.h"
#include "exceptions.h"

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

	/* The same magnitude and phase as the package's constructor, so the phase is wrapped into (-pi, pi], as it is
	 * there. */
	*out_re = log(complex_compute_magnitude(re, im));
	*out_im = complex_compute_phase(re, im);
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

	if (complex_create_result(return_value, sinh(x) * cos(y), cosh(x) * sin(y), "compute hyperbolic sine of Complex")
		== FAILURE) {
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

	if (complex_create_result(return_value, cosh(x) * cos(y), sinh(x) * sin(y), "compute hyperbolic cosine of Complex")
		== FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ complex_calc_tanh
 *
 * Matches the PHP package's Complex::calcTanh(): the hyperbolic tangent of x + iy, by Kahan's algorithm, which is
 * accurate and can't overflow, unlike sinh(z) / cosh(z). Declared in complex_internal.h, as tan()
 * (complex_trigonometric.c) uses it too.
 */
void complex_calc_tanh(double x, double y, double *out_re, double *out_im)
{
	/* For a large real part, tanh(z) = +-1 + i*4*sin(y)*cos(y)*e^(-2|x|), to within rounding. Multiplying by e^(-|x|)
	 * twice lets the imaginary part underflow gradually, instead of e^(-2|x|) underflowing to 0 first. */
	if (fabs(x) >= 22) {
		double exp_minus_x = exp(-fabs(x));
		*out_re = x > 0 ? 1.0 : -1.0;
		*out_im = 4 * sin(y) * cos(y) * exp_minus_x * exp_minus_x;
		return;
	}

	/* Otherwise, with t = tan(y), beta = 1 + t^2, s = sinh(x) and rho = sqrt(1 + s^2) = cosh(x):
	 * tanh(z) = (beta*rho*s + i*t) / (1 + beta*s^2). */
	double t = tan(y);
	double beta = 1 + t * t;
	double s = sinh(x);
	double rho = sqrt(1 + s * s);
	double denom = 1 + beta * s * s;
	*out_re = beta * rho * s / denom;
	*out_im = t / denom;
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::tanh(): Complex
 *
 * Matches the PHP package's Complex::tanh(), via complex_calc_tanh().
 */
PHP_METHOD(OceanMoon_Math_Complex, tanh)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double x, y;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &x, &y);

	double out_re, out_im;
	complex_calc_tanh(x, y, &out_re, &out_im);

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

	/* z^2 + 1, checking z^2 is representable, as the package's mul() does. */
	if (complex_parts_overflow(x * x - y * y, x * y + y * x)) {
		math_throw_overflow("multiply Complex numbers");
		math_rethrow_overflow("compute inverse hyperbolic sine of Complex");
		RETURN_THROWS();
	}
	double z2_plus_1_re = (x * x - y * y) + 1.0;
	double z2_plus_1_im = (x * y + y * x) + 0.0;

	double sqrt_re, sqrt_im;
	complex_calc_sqrt(z2_plus_1_re, z2_plus_1_im, &sqrt_re, &sqrt_im);

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

	/* z^2 - 1, checking z^2 is representable, as the package's mul() does. */
	if (complex_parts_overflow(x * x - y * y, x * y + y * x)) {
		math_throw_overflow("multiply Complex numbers");
		math_rethrow_overflow("compute inverse hyperbolic cosine of Complex");
		RETURN_THROWS();
	}
	double z2_minus_1_re = (x * x - y * y) - 1.0;
	double z2_minus_1_im = x * y + y * x;

	double sqrt_re, sqrt_im;
	complex_calc_sqrt(z2_minus_1_re, z2_minus_1_im, &sqrt_re, &sqrt_im);

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

	/* atanh(z) is undefined at z = +-1. */
	if (y == 0.0 && fabs(x) == 1.0) {
		zend_throw_exception(
			math_ce_ArithmeticException,
			x > 0 ? "Cannot compute inverse hyperbolic tangent of 1." : "Cannot compute inverse hyperbolic tangent of -1.",
			0
		);
		RETURN_THROWS();
	}

	/* 1 + z, 1 - z. */
	double one_plus_z_re = 1 + x, one_plus_z_im = y;
	double one_minus_z_re = 1 - x, one_minus_z_im = -y;

	double div_re, div_im;
	if (complex_calc_div(one_plus_z_re, one_plus_z_im, one_minus_z_re, one_minus_z_im, &div_re, &div_im) == FAILURE) {
		math_rethrow_overflow("compute inverse hyperbolic tangent of Complex");
		RETURN_THROWS();
	}

	double ln_re, ln_im;
	if (c_ln(div_re, div_im, &ln_re, &ln_im) == FAILURE) {
		RETURN_THROWS();
	}

	/* Multiply by 0.5 with the full product (a+bi)(0.5+0i), as the package's mul() does, so a zero part gets the
	 * same sign. */
	if (complex_create(return_value, ln_re * 0.5 - ln_im * 0.0, ln_re * 0.0 + ln_im * 0.5) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */
