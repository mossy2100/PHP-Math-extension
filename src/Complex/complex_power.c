/*
 * complex_power.c
 *
 * Power and root methods for OceanMoon\Math\Complex: pow(), sqr(), roots(), sqrt(). Mirrors the
 * "Power methods"/"Root methods" regions of the PHP package's Complex class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <float.h>
#include <math.h>

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "complex_internal.h"
#include "../../oceanmoon_math_arginfo.h"
#include "exceptions.h"
#include "floats.h"

static zend_result complex_calc_pow_general(
	zend_object *base_obj, double real, double imag, double other_real, double other_imag, double *out_real,
	double *out_imag
);

/* {{{ complex_calc_pow
 *
 * The computational core of pow(), factored out so complex_do_operation() (complex_operators.c)
 * can reuse it for the `**` operator without going through a full PHP method dispatch - matching
 * the "read the base's own magnitude/phase" behavior below exactly, base_obj plays the role
 * Z_OBJ_P(ZEND_THIS) plays for the method call.
 *
 * Matches the PHP package's Complex::pow(): z^w. Only the principal value is returned for
 * multivalued cases. Uses the same shortcuts as the PHP source (0 exponent, 0 base, exponent 1/
 * 2/-1, base e), then exponentiation by squaring for an integer exponent, falling back to
 * z^w = e^(w * ln(z)) for the general case - calling
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

	/* As in the package, an overflow at any later step is reported as an overflow of pow() itself. */
	if (complex_calc_pow_general(base_obj, real, imag, other_real, other_imag, out_real, out_imag) == FAILURE) {
		math_rethrow_overflow("raise the given Complex to the given power");
		return FAILURE;
	}
	if (complex_parts_overflow(*out_real, *out_imag)) {
		math_throw_overflow("raise the given Complex to the given power");
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

/* {{{ complex_calc_pow_general
 *
 * The rest of complex_calc_pow(), once the exponent 0 and 1 and zero base cases are handled. Matches the PHP
 * package's Complex::calcPow(). An overflow throws with the message of the step that overflowed, which
 * complex_calc_pow() then replaces.
 */
static zend_result complex_calc_pow_general(
	zend_object *base_obj, double real, double imag, double other_real, double other_imag, double *out_real,
	double *out_imag
)
{
	/* Exponent 2: delegate to sqr(). The caller checks the result. */
	if (other_real == 2.0 && other_imag == 0.0) {
		*out_real = real * real - imag * imag;
		*out_imag = real * imag + imag * real;
		return SUCCESS;
	}

	/* Exponent 0.5: delegate to sqrt(), which is more accurate. */
	if (other_real == 0.5 && other_imag == 0.0) {
		complex_calc_sqrt(real, imag, out_real, out_imag);
		return SUCCESS;
	}

	/* Exponent -1: delegate to recip(). Safe - base != 0 already established above. */
	if (other_real == -1.0 && other_imag == 0.0) {
		return complex_calc_div(1.0, 0.0, real, imag, out_real, out_imag);
	}

	/* Base e: z^w = e^w, saving unnecessary ln()/mul() calls. */
	if (real == M_E && imag == 0.0) {
		return complex_calc_exp(other_real, other_imag, out_real, out_imag);
	}

	/* Integer exponent: exponentiation by squaring, which is exact where the products are. Each product is
	 * checked, as the package's mul() calls are, so an overflow fails at the same step with the same message. */
	if (other_imag == 0.0 && math_floats_is_int(other_real)) {
		double x_real = real, x_imag = imag;
		if (other_real < 0 && complex_calc_div(1.0, 0.0, real, imag, &x_real, &x_imag) == FAILURE) {
			return FAILURE;
		}

		double result_real = 1.0, result_imag = 0.0;
		double y = fabs(other_real);
		while (y > 0) {
			if (fmod(y, 2) == 1.0) {
				double r = result_real * x_real - result_imag * x_imag;
				double i = result_real * x_imag + result_imag * x_real;
				if (complex_parts_overflow(r, i)) {
					math_throw_overflow("multiply the given Complex numbers");
					return FAILURE;
				}
				result_real = r;
				result_imag = i;
			}
			y = floor(y / 2);
			if (y > 0) {
				double r = x_real * x_real - x_imag * x_imag;
				double i = x_real * x_imag + x_imag * x_real;
				if (complex_parts_overflow(r, i)) {
					math_throw_overflow("multiply the given Complex numbers");
					return FAILURE;
				}
				x_real = r;
				x_imag = i;
			}
		}

		*out_real = result_real;
		*out_imag = result_imag;
		return SUCCESS;
	}

	/* General solution: z^w = e^(w * ln(z)). */
	double ln_real, ln_imag;
	if (complex_calc_ln(base_obj, &ln_real, &ln_imag) == FAILURE) {
		return FAILURE;
	}

	double mul_real = other_real * ln_real - other_imag * ln_imag;
	double mul_imag = other_real * ln_imag + other_imag * ln_real;
	if (complex_parts_overflow(mul_real, mul_imag)) {
		math_throw_overflow("multiply the given Complex numbers");
		return FAILURE;
	}

	return complex_calc_exp(mul_real, mul_imag, out_real, out_imag);
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

	double out_real = real * real - imag * imag;
	double out_imag = real * imag + imag * real;
	if (complex_create_result(return_value, out_real, out_imag, "square the given Complex") == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::roots(int $degree): array
 *
 * Matches the PHP package's Complex::roots(): all $degree roots via De Moivre's theorem, reading
 * the magnitude/phase computed properties (not recomputing them) to match the PHP source exactly.
 * Roots that lie on an axis are exact, and the square roots are sqrt() and its negative.
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

	/* Special case of 0: every root is 0. Degree 1: the only root is this number. */
	if ((real == 0.0 && imag == 0.0) || degree == 1) {
		zval root;
		bool is_zero = real == 0.0 && imag == 0.0;
		if (complex_create(&root, is_zero ? 0.0 : real, is_zero ? 0.0 : imag) == FAILURE) {
			zval_ptr_dtor(return_value);
			RETURN_THROWS();
		}
		add_next_index_zval(return_value, &root);
		return;
	}

	/* Degree 2: sqrt() and its negative, so the results agree. */
	if (degree == 2) {
		double root_real, root_imag;
		complex_calc_sqrt(real, imag, &root_real, &root_imag);
		zval root, neg_root;
		if (complex_create(&root, root_real, root_imag) == FAILURE) {
			zval_ptr_dtor(return_value);
			RETURN_THROWS();
		}
		add_next_index_zval(return_value, &root);
		if (complex_create(&neg_root, -root_real, -root_imag) == FAILURE) {
			zval_ptr_dtor(return_value);
			RETURN_THROWS();
		}
		add_next_index_zval(return_value, &neg_root);
		return;
	}

	double magnitude, phase;
	complex_read_magnitude_phase(Z_OBJ_P(ZEND_THIS), &magnitude, &phase);

	/* Raising to the power 1/n is inexact, as 1/n is rounded, e.g. 64 ** (1/3) is 3.9999999999999996. So, refine it
	 * with one step of Newton's method, keeping whichever is closer. */
	double n = (double) degree;
	double root_mag = pow(magnitude, 1.0 / n);
	double refined = root_mag - (pow(root_mag, n) - magnitude) / (n * pow(root_mag, n - 1));
	if (fabs(pow(refined, n) - magnitude) < fabs(pow(root_mag, n) - magnitude)) {
		root_mag = refined;
	}
	double theta = phase / (double) degree;
	double delta = (2 * M_PI) / (double) degree;

	/* If this number lies on an axis, get its phase as a whole number of quarter turns. */
	bool on_axis = true;
	zend_long quarter_turns = 0;
	if (imag == 0.0) {
		quarter_turns = real > 0 ? 0 : 2;
	} else if (real == 0.0) {
		quarter_turns = imag > 0 ? 1 : -1;
	} else {
		on_axis = false;
	}

	for (zend_long k = 0; k < degree; k++) {
		zval root;
		zend_result result;
		/* A root that lies on an axis is the root magnitude times 1, i, -1 or -i, so calculate it exactly. */
		if (on_axis && (quarter_turns + 4 * k) % degree == 0) {
			switch (((quarter_turns + 4 * k) / degree) % 4) {
				case 0:
					result = complex_create(&root, root_mag, 0.0);
					break;
				case 1:
					result = complex_create(&root, 0.0, root_mag);
					break;
				case 2:
					result = complex_create(&root, -root_mag, 0.0);
					break;
				default:
					result = complex_create(&root, 0.0, -root_mag);
					break;
			}
		} else {
			double root_phase = theta + (double) k * delta;
			result = complex_from_polar(&root, root_mag, root_phase);
		}
		if (result == FAILURE) {
			zval_ptr_dtor(return_value);
			RETURN_THROWS();
		}
		add_next_index_zval(return_value, &root);
	}
}
/* }}} */

/* {{{ complex_calc_sqrt
 *
 * Matches the PHP package's Complex::sqrt(): a numerically stable formula rather than the polar form, so the
 * result is exact on the axes. With z = a + bi and t = √((|z| + |a|) / 2), √z = t + (b / 2t)i if a ≥ 0, or
 * |b| / 2t ± ti, taking the sign of b, if a < 0. As with the phase, a zero b counts as positive.
 */
void complex_calc_sqrt(double real, double imag, double *out_real, double *out_imag)
{
	if (real == 0.0 && imag == 0.0) {
		*out_real = 0.0;
		*out_imag = 0.0;
		return;
	}

	double magnitude = complex_compute_magnitude(real, imag);
	double abs_real = fabs(real);
	double sum = magnitude + abs_real;
	double t;
	if (zend_isinf(sum)) {
		/* Scale down to avoid overflow. */
		t = 2 * sqrt(magnitude / 8 + abs_real / 8);
	} else if (sum < 2 * DBL_MIN) {
		/* Scale up, as halving a subnormal number loses precision. */
		t = sqrt(2 * sum) / 2;
	} else {
		t = sqrt(sum / 2);
	}

	if (real >= 0) {
		*out_real = t;
		*out_imag = imag == 0.0 ? 0.0 : imag / (2 * t);
	} else {
		*out_real = fabs(imag) / (2 * t);
		*out_imag = imag < 0 ? -t : t;
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::sqrt(): Complex
 *
 * Matches the PHP package's Complex::sqrt(): the principal square root. For both square roots, call roots(2)
 * instead.
 */
PHP_METHOD(OceanMoon_Math_Complex, sqrt)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double real, imag;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &real, &imag);

	double out_real, out_imag;
	complex_calc_sqrt(real, imag, &out_real, &out_imag);

	if (complex_create(return_value, out_real, out_imag) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */
