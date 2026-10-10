/*
 * complex_trigonometric.c
 *
 * Trigonometric and inverse trigonometric methods for OceanMoon\Math\Complex: sin(), cos(), tan(),
 * asin(), acos(), atan(). Mirrors the "Trigonometric methods"/"Inverse trigonometric methods"
 * regions of the PHP package's Complex class.
 *
 * The inverse methods (asin/acos/atan) are each a short chain of add/sub/mul/div/sqrt/ln in the
 * package source (e.g. `$iz->add($oneMinusZ2->sqrt())->ln()->mul(new self(0, -1))`). Rather than
 * promoting each intermediate value to a real Complex object and going through full method
 * dispatch for every step (as complex_operators.c's `**` case must, since complex_calc_pow() needs
 * a real object to read cached magnitude/phase off), this file works the chain in plain doubles via
 * the c_*() helpers below - there's no cache to benefit from for a value that's freshly computed
 * and only used once. This also means these helpers skip the well-known-value shortcuts
 * complex_calc_ln()/pow()'s sqrt step take (real() sqrt via complex_from_polar()) - fine here,
 * since an intermediate value from one of these formulas landing exactly on a shortcut input (e.g.
 * exactly 1+0i) is not a correctness concern, just a missed micro-optimization, and every caller of
 * these methods in the test suite compares results with EPSILON tolerance, not bit-for-bit.
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

/* {{{ c_sqrt
 *
 * Principal square root of a plain (real, imaginary) pair, via magnitude/phase - the same
 * approach as OceanMoon_Math_Complex::sqrt() (complex_power.c), but operating on raw doubles
 * rather than reading a real object's (possibly cached) magnitude/phase properties.
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

/* {{{ OceanMoon\Math\Complex::sin(): Complex
 *
 * Matches the PHP package's Complex::sin(): sin(z) = sin(x)cosh(y) + i·cos(x)sinh(y), z = x + iy.
 */
PHP_METHOD(OceanMoon_Math_Complex, sin)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double x, y;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &x, &y);

	if (complex_create(return_value, sin(x) * cosh(y), cos(x) * sinh(y)) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::cos(): Complex
 *
 * Matches the PHP package's Complex::cos(): cos(z) = cos(x)cosh(y) - i·sin(x)sinh(y), z = x + iy.
 */
PHP_METHOD(OceanMoon_Math_Complex, cos)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double x, y;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &x, &y);

	if (complex_create(return_value, cos(x) * cosh(y), -sin(x) * sinh(y)) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::tan(): Complex
 *
 * Matches the PHP package's Complex::tan(): tan(z) = sin(z) / cos(z).
 */
PHP_METHOD(OceanMoon_Math_Complex, tan)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double x, y;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &x, &y);

	/* sin(z)/cos(z), checking each is finite, as the package's sin() and cos() calls do. */
	double sin_re = sin(x) * cosh(y), sin_im = cos(x) * sinh(y);
	if (!zend_finite(sin_re) || !zend_finite(sin_im)) {
		complex_throw_non_finite(sin_re, sin_im);
		RETURN_THROWS();
	}
	double cos_re = cos(x) * cosh(y), cos_im = -sin(x) * sinh(y);
	if (!zend_finite(cos_re) || !zend_finite(cos_im)) {
		complex_throw_non_finite(cos_re, cos_im);
		RETURN_THROWS();
	}

	double out_re, out_im;
	if (complex_calc_div(sin_re, sin_im, cos_re, cos_im, &out_re, &out_im) == FAILURE) {
		RETURN_THROWS();
	}

	if (complex_create(return_value, out_re, out_im) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::asin(): Complex
 *
 * Matches the PHP package's Complex::asin(): asin(z) = -i·ln(iz + √(1-z²)).
 */
PHP_METHOD(OceanMoon_Math_Complex, asin)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double x, y;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &x, &y);

	/* iz = -y + xi. */
	double iz_re = -y, iz_im = x;

	/* 1 - z^2. */
	double one_minus_z2_re = 1.0 - (x * x - y * y);
	double one_minus_z2_im = -(2 * x * y);

	double sqrt_re, sqrt_im;
	c_sqrt(one_minus_z2_re, one_minus_z2_im, &sqrt_re, &sqrt_im);

	double sum_re = iz_re + sqrt_re, sum_im = iz_im + sqrt_im;

	double ln_re, ln_im;
	if (c_ln(sum_re, sum_im, &ln_re, &ln_im) == FAILURE) {
		RETURN_THROWS();
	}

	/* Multiply by -i with the full product (a+bi)(0-i), as the package's mul() does, so a zero part gets the same
	 * sign: -0 + 0 is +0, where negating it directly would give -0. */
	if (complex_create(return_value, ln_re * 0.0 - ln_im * -1.0, ln_re * -1.0 + ln_im * 0.0) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::acos(): Complex
 *
 * Matches the PHP package's Complex::acos(): acos(z) = -i·ln(z + i·√(1-z²)).
 */
PHP_METHOD(OceanMoon_Math_Complex, acos)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double x, y;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &x, &y);

	/* 1 - z^2. */
	double one_minus_z2_re = 1.0 - (x * x - y * y);
	double one_minus_z2_im = -(2 * x * y);

	double sqrt_re, sqrt_im;
	c_sqrt(one_minus_z2_re, one_minus_z2_im, &sqrt_re, &sqrt_im);

	/* i * sqrt: (a+bi)i = -b + ai. */
	double i_sqrt_re = -sqrt_im, i_sqrt_im = sqrt_re;

	double sum_re = x + i_sqrt_re, sum_im = y + i_sqrt_im;

	double ln_re, ln_im;
	if (c_ln(sum_re, sum_im, &ln_re, &ln_im) == FAILURE) {
		RETURN_THROWS();
	}

	/* Multiply by -i with the full product (a+bi)(0-i), as the package's mul() does, so a zero part gets the same
	 * sign: -0 + 0 is +0, where negating it directly would give -0. */
	if (complex_create(return_value, ln_re * 0.0 - ln_im * -1.0, ln_re * -1.0 + ln_im * 0.0) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::atan(): Complex
 *
 * Matches the PHP package's Complex::atan(): atan(z) = (-i/2)·ln((i-z)/(i+z)).
 */
PHP_METHOD(OceanMoon_Math_Complex, atan)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double x, y;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &x, &y);

	/* i - z = -x + (1-y)i. i + z = x + (1+y)i. */
	double i_minus_z_re = -x, i_minus_z_im = 1 - y;
	double i_plus_z_re = x, i_plus_z_im = 1 + y;

	double div_re, div_im;
	if (complex_calc_div(i_minus_z_re, i_minus_z_im, i_plus_z_re, i_plus_z_im, &div_re, &div_im) == FAILURE) {
		RETURN_THROWS();
	}

	double ln_re, ln_im;
	if (c_ln(div_re, div_im, &ln_re, &ln_im) == FAILURE) {
		RETURN_THROWS();
	}

	/* Multiply by -i/2 with the full product (a+bi)(0-0.5i), as the package's mul() does, so a zero part gets the
	 * same sign. */
	if (complex_create(return_value, ln_re * 0.0 - ln_im * -0.5, ln_re * -0.5 + ln_im * 0.0) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */
