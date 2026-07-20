/*
 * complex_power.c
 *
 * Power and root methods for OceanMoon\Math\Complex: pow(), sqr(), roots(), sqrt(). Mirrors the
 * "Power methods"/"Root methods" regions of the PHP package's Complex class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <math.h>

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "complex_internal.h"
#include "complex_arginfo.h"
#include "exceptions.h"

/* {{{ complex_calc_pow
 *
 * The computational core of pow(), factored out so complex_do_operation() (complex_operators.c)
 * can reuse it for the `**` operator without going through a full PHP method dispatch -- matching
 * the "read the base's own magnitude/phase" behavior below exactly, base_obj plays the role
 * Z_OBJ_P(ZEND_THIS) plays for the method call.
 *
 * Matches the PHP package's Complex::pow(): z^w. Only the principal value is returned for
 * multivalued cases. Uses the same shortcuts as the PHP source (0 exponent, 0 base, exponent 1/
 * 2/-1, base e), falling back to z^w = e^(w * ln(z)) for the general case -- calling
 * complex_calc_exp()/complex_calc_ln() directly rather than going through a full PHP method
 * dispatch.
 *
 * Returns FAILURE (with an exception already thrown) for 0 raised to a complex or negative power.
 */
zend_result complex_calc_pow(zend_object *base_obj, double other_real, double other_imag, double *out_real, double *out_imag)
{
	double real, imag;
	complex_read_parts(base_obj, &real, &imag);

	/* Exponent 0: any number to power 0 is 1 (including 0^0 by convention). */
	if (other_real == 0.0 && other_imag == 0.0) {
		*out_real = 1.0;
		*out_imag = 0.0;
		return SUCCESS;
	}

	/* Base 0. */
	if (real == 0.0 && imag == 0.0) {
		if (other_imag != 0.0) {
			zend_throw_exception(math_ce_ArithmeticException, "Cannot raise zero to a complex power.", 0);
			return FAILURE;
		}
		if (other_real < 0) {
			zend_throw_exception(math_ce_ArithmeticException, "Cannot raise zero to a negative power.", 0);
			return FAILURE;
		}
		*out_real = 0.0;
		*out_imag = 0.0;
		return SUCCESS;
	}

	/* Exponent 1: any number to power 1 is itself. */
	if (other_real == 1.0 && other_imag == 0.0) {
		*out_real = real;
		*out_imag = imag;
		return SUCCESS;
	}

	/* Exponent 2: delegate to sqr(). */
	if (other_real == 2.0 && other_imag == 0.0) {
		*out_real = real * real - imag * imag;
		*out_imag = 2 * real * imag;
		return SUCCESS;
	}

	/* Exponent -1: delegate to inv(). Safe -- base != 0 already established above. */
	if (other_real == -1.0 && other_imag == 0.0) {
		double f = (real * real) + (imag * imag);
		*out_real = real / f;
		*out_imag = -imag / f;
		return SUCCESS;
	}

	/* Base e: z^w = e^w, saving unnecessary ln()/mul() calls. */
	if (real == M_E && imag == 0.0) {
		complex_calc_exp(other_real, other_imag, out_real, out_imag);
		return SUCCESS;
	}

	/* General solution: z^w = e^(w * ln(z)). */
	double ln_real, ln_imag;
	if (complex_calc_ln(base_obj, &ln_real, &ln_imag) == FAILURE) {
		return FAILURE;
	}

	double mul_real = other_real * ln_real - other_imag * ln_imag;
	double mul_imag = other_real * ln_imag + other_imag * ln_real;

	complex_calc_exp(mul_real, mul_imag, out_real, out_imag);
	return SUCCESS;
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::pow(Complex|float $other): Complex */
PHP_METHOD(OceanMoon_Math_Complex, pow)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(other)
	ZEND_PARSE_PARAMETERS_END();

	double other_real, other_imag;
	if (complex_read_other(other, &other_real, &other_imag) == FAILURE) {
		RETURN_THROWS();
	}

	double out_real, out_imag;
	if (complex_calc_pow(Z_OBJ_P(ZEND_THIS), other_real, other_imag, &out_real, &out_imag) == FAILURE) {
		RETURN_THROWS();
	}

	if (complex_create(return_value, out_real, out_imag) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::sqr(): Complex
 *
 * Matches the PHP package's Complex::sqr(): this * this. Equivalent to pow(2), but more
 * efficient/readable.
 */
PHP_METHOD(OceanMoon_Math_Complex, sqr)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double real, imag;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &real, &imag);

	if (complex_create(return_value, real * real - imag * imag, 2 * real * imag) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::roots(int $degree): array
 *
 * Matches the PHP package's Complex::roots(): all $degree roots via De Moivre's theorem, reading
 * the magnitude/phase computed properties (not recomputing them) to match the PHP source exactly.
 */
PHP_METHOD(OceanMoon_Math_Complex, roots)
{
	zend_long degree;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(degree)
	ZEND_PARSE_PARAMETERS_END();

	if (degree <= 0) {
		zend_string *msg = strpprintf(0, "Cannot compute roots of degree: " ZEND_LONG_FMT ". Must be positive.", degree);
		zend_throw_exception(spl_ce_DomainException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		RETURN_THROWS();
	}

	double real, imag;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &real, &imag);

	array_init(return_value);

	/* Special case of 0: every root is 0. */
	if (real == 0.0 && imag == 0.0) {
		zval root;
		if (complex_create(&root, 0.0, 0.0) == FAILURE) {
			zval_ptr_dtor(return_value);
			RETURN_THROWS();
		}
		add_next_index_zval(return_value, &root);
		return;
	}

	double magnitude, phase;
	complex_read_magnitude_phase(Z_OBJ_P(ZEND_THIS), &magnitude, &phase);

	double root_mag = pow(magnitude, 1.0 / (double) degree);
	double theta = phase / (double) degree;
	double delta = (2 * M_PI) / (double) degree;

	for (zend_long k = 0; k < degree; k++) {
		double root_phase = theta + (double) k * delta;
		zval root;
		if (complex_from_polar(&root, root_mag, root_phase) == FAILURE) {
			zval_ptr_dtor(return_value);
			RETURN_THROWS();
		}
		add_next_index_zval(return_value, &root);
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::sqrt(): Complex
 *
 * Matches the PHP package's Complex::sqrt(): the principal square root, via magnitude/phase. For
 * both square roots, call roots(2) instead.
 */
PHP_METHOD(OceanMoon_Math_Complex, sqrt)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double magnitude, phase;
	complex_read_magnitude_phase(Z_OBJ_P(ZEND_THIS), &magnitude, &phase);

	if (complex_from_polar(return_value, sqrt(magnitude), phase / 2.0) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */
