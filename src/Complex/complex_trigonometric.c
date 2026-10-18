/*
 * complex_trigonometric.c
 *
 * Trigonometric and inverse trigonometric methods for OceanMoon\Math\Complex: sin(), cos(), tan(),
 * asin(), acos(), atan(). Mirrors the "Trigonometric methods"/"Inverse trigonometric methods"
 * regions of the PHP package's Complex class.
 *
 * The inverse methods compute with complex_calc_asinh(), complex_calc_acos() and complex_calc_atanh()
 * (complex_catrig.c).
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

/* {{{ OceanMoon\Math\Complex::sin(): Complex
 *
 * Matches the PHP package's Complex::sin(): sin(z) = sin(x)cosh(y) + i·cos(x)sinh(y), z = x + iy.
 */
PHP_METHOD(OceanMoon_Math_Complex, sin)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double x, y;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &x, &y);

	if (complex_create_result(
		return_value, sin(x) * cosh(y), cos(x) * sinh(y), "compute sine of the given Complex"
	) == FAILURE) {
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

	if (complex_create_result(return_value, cos(x) * cosh(y), -sin(x) * sinh(y), "compute cosine of the given Complex")
		== FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::tan(): Complex
 *
 * Matches the PHP package's Complex::tan(): tan(z) = -i*tanh(iz), where iz = -y + ix. If tanh(iz) = u + iv, then
 * tan(z) = v - iu.
 */
PHP_METHOD(OceanMoon_Math_Complex, tan)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double x, y;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &x, &y);

	double u, v;
	complex_calc_tanh(-y, x, &u, &v);

	if (complex_create(return_value, v, -u) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::asin(): Complex
 *
 * Matches the PHP package's Complex::asin(): asinh(y + ix) with its parts swapped.
 */
PHP_METHOD(OceanMoon_Math_Complex, asin)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double x, y;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &x, &y);

	/* asin(x + iy) is asinh(y + ix) with its parts swapped. */
	double out_re, out_im;
	complex_calc_asinh(y, x, &out_im, &out_re);

	if (complex_create(return_value, out_re, out_im) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::acos(): Complex
 *
 * Matches the PHP package's Complex::acos().
 */
PHP_METHOD(OceanMoon_Math_Complex, acos)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double x, y;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &x, &y);

	double out_re, out_im;
	complex_calc_acos(x, y, &out_re, &out_im);

	if (complex_create(return_value, out_re, out_im) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::atan(): Complex
 *
 * Matches the PHP package's Complex::atan(): atanh(y + ix) with its parts swapped.
 */
PHP_METHOD(OceanMoon_Math_Complex, atan)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double x, y;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &x, &y);

	/* atan(z) is undefined at z = +-i. */
	if (x == 0.0 && fabs(y) == 1.0) {
		zend_throw_exception(
			math_ce_ArithmeticException,
			y > 0 ? "Cannot compute inverse tangent of i." : "Cannot compute inverse tangent of -i.",
			0
		);
		RETURN_THROWS();
	}

	/* atan(x + iy) is atanh(y + ix) with its parts swapped. */
	double out_re, out_im;
	complex_calc_atanh(y, x, &out_im, &out_re);

	if (complex_create(return_value, out_re, out_im) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */
