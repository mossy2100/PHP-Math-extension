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

/* {{{ OceanMoon\Math\Complex::pow(Complex|float $other): Complex
 *
 * Matches the PHP package's Complex::pow(): z^w. Only the principal value is returned for
 * multivalued cases. Uses the same shortcuts as the PHP source (0 exponent, 0 base, exponent 1/
 * 2/-1, base e), falling back to z^w = e^(w * ln(z)) for the general case -- calling
 * complex_calc_exp()/complex_calc_ln() directly (complex_transcendental.c) rather than going
 * through a full PHP method dispatch.
 */
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

	double real, imag;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &real, &imag);

	/* Exponent 0: any number to power 0 is 1 (including 0^0 by convention). */
	if (other_real == 0.0 && other_imag == 0.0) {
		if (complex_create(return_value, 1.0, 0.0) == FAILURE) {
			RETURN_THROWS();
		}
		return;
	}

	/* Base 0. */
	if (real == 0.0 && imag == 0.0) {
		if (other_imag != 0.0) {
			zend_throw_exception(math_ce_ArithmeticException, "Cannot raise zero to a complex power.", 0);
			RETURN_THROWS();
		}
		if (other_real < 0) {
			zend_throw_exception(math_ce_ArithmeticException, "Cannot raise zero to a negative power.", 0);
			RETURN_THROWS();
		}
		if (complex_create(return_value, 0.0, 0.0) == FAILURE) {
			RETURN_THROWS();
		}
		return;
	}

	/* Exponent 1: any number to power 1 is itself. */
	if (other_real == 1.0 && other_imag == 0.0) {
		if (complex_create(return_value, real, imag) == FAILURE) {
			RETURN_THROWS();
		}
		return;
	}

	/* Exponent 2: delegate to sqr(). */
	if (other_real == 2.0 && other_imag == 0.0) {
		if (complex_create(return_value, real * real - imag * imag, 2 * real * imag) == FAILURE) {
			RETURN_THROWS();
		}
		return;
	}

	/* Exponent -1: delegate to inv(). Safe -- base != 0 already established above. */
	if (other_real == -1.0 && other_imag == 0.0) {
		double f = (real * real) + (imag * imag);
		if (complex_create(return_value, real / f, -imag / f) == FAILURE) {
			RETURN_THROWS();
		}
		return;
	}

	/* Base e: z^w = e^w, saving unnecessary ln()/mul() calls. */
	if (real == M_E && imag == 0.0) {
		double exp_real, exp_imag;
		complex_calc_exp(other_real, other_imag, &exp_real, &exp_imag);
		if (complex_create(return_value, exp_real, exp_imag) == FAILURE) {
			RETURN_THROWS();
		}
		return;
	}

	/* General solution: z^w = e^(w * ln(z)). */
	double ln_real, ln_imag;
	if (complex_calc_ln(Z_OBJ_P(ZEND_THIS), &ln_real, &ln_imag) == FAILURE) {
		RETURN_THROWS();
	}

	double mul_real = other_real * ln_real - other_imag * ln_imag;
	double mul_imag = other_real * ln_imag + other_imag * ln_real;

	double exp_real, exp_imag;
	complex_calc_exp(mul_real, mul_imag, &exp_real, &exp_imag);

	if (complex_create(return_value, exp_real, exp_imag) == FAILURE) {
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
