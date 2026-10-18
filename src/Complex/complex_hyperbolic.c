/*
 * complex_hyperbolic.c
 *
 * Hyperbolic and inverse hyperbolic methods for OceanMoon\Math\Complex: sinh(), cosh(), tanh(),
 * asinh(), acosh(), atanh(). Mirrors the "Hyperbolic methods"/"Inverse hyperbolic methods" regions
 * of the PHP package's Complex class. The inverse methods compute with complex_calc_asinh(),
 * complex_calc_acos() and complex_calc_atanh() (complex_catrig.c).
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

/* {{{ OceanMoon\Math\Complex::sinh(): Complex
 *
 * Matches the PHP package's Complex::sinh(): sinh(z) = sinh(x)cos(y) + i·cosh(x)sin(y), z = x + iy.
 */
PHP_METHOD(OceanMoon_Math_Complex, sinh)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double x, y;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &x, &y);

	if (complex_create_result(
		return_value, sinh(x) * cos(y), cosh(x) * sin(y), "compute hyperbolic sine of the given Complex"
	) == FAILURE) {
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

	if (complex_create_result(
		return_value, cosh(x) * cos(y), sinh(x) * sin(y), "compute hyperbolic cosine of the given Complex"
	) == FAILURE) {
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
 * Matches the PHP package's Complex::asinh().
 */
PHP_METHOD(OceanMoon_Math_Complex, asinh)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double x, y;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &x, &y);

	double out_re, out_im;
	complex_calc_asinh(x, y, &out_re, &out_im);

	if (complex_create(return_value, out_re, out_im) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::acosh(): Complex
 *
 * Matches the PHP package's Complex::acosh(): ±i·acos(z), with the sign chosen so the real part isn't
 * negative.
 */
PHP_METHOD(OceanMoon_Math_Complex, acosh)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double x, y;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &x, &y);

	double acos_re, acos_im;
	complex_calc_acos(x, y, &acos_re, &acos_im);
	double out_re = fabs(acos_im);
	double out_im = copysign(acos_re, y);

	if (complex_create(return_value, out_re, out_im) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::atanh(): Complex
 *
 * Matches the PHP package's Complex::atanh().
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

	double out_re, out_im;
	complex_calc_atanh(x, y, &out_re, &out_im);

	if (complex_create(return_value, out_re, out_im) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */
