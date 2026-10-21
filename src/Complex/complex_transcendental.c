/*
 * complex_transcendental.c
 *
 * Transcendental methods for OceanMoon\Math\Complex: exp(), ln(), and log(). Mirrors the
 * "Transcendental methods" region of the PHP package's Complex class.
 *
 * complex_calc_exp()/complex_calc_ln() are the computational cores, factored out so pow()
 * (complex_power.c) and log() (this file) can call them directly as pure functions - reusing
 * exp()/ln()'s shortcuts without going through a full PHP method dispatch (e.g. `$other->exp()`,
 * `$this->ln()`).
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <math.h>

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "complex_internal.h"
#include "../../oceanmoon_math_arginfo.h"
#include "exceptions.h"
#include "floats.h"

/* {{{ complex_calc_exp
 *
 * Matches the PHP package's Complex::exp(): e^z. Uses the same shortcuts (matching well-known
 * values exactly, including the two Eulerian identities), falling back to Euler's formula
 * (e^(a+bi) = e^a * (cos(b) + i*sin(b))) for the general case, like the package's
 * fromPolar(exp(real), imag). Returns FAILURE (with OverflowException already thrown) if e^a overflows.
 */
zend_result complex_calc_exp(double real, double imag, double *out_real, double *out_imag)
{
	if (imag == 0.0) {
		if (real == 0.0) {
			*out_real = 1.0;
			*out_imag = 0.0;
			return SUCCESS;
		}
		if (real == 1.0) {
			*out_real = M_E;
			*out_imag = 0.0;
			return SUCCESS;
		}
		if (real == M_LN2) {
			*out_real = 2.0;
			*out_imag = 0.0;
			return SUCCESS;
		}
		if (real == log(M_PI)) {
			*out_real = M_PI;
			*out_imag = 0.0;
			return SUCCESS;
		}
		if (real == M_LN10) {
			*out_real = 10.0;
			*out_imag = 0.0;
			return SUCCESS;
		}
	} else if (real == 0.0) {
		/* Eulerian identities: e^(i*pi) = -1, e^(i*tau) = 1. */
		if (imag == M_PI) {
			*out_real = -1.0;
			*out_imag = 0.0;
			return SUCCESS;
		}
		if (imag == 2 * M_PI) {
			*out_real = 1.0;
			*out_imag = 0.0;
			return SUCCESS;
		}
	}

	/* General solution: Euler's formula. */
	double mag = exp(real);
	if (!zend_finite(mag)) {
		math_throw_overflow("compute exponential of the given Complex");
		return FAILURE;
	}
	*out_real = mag * cos(imag);
	*out_imag = mag * sin(imag);
	return SUCCESS;
}
/* }}} */

/* {{{ complex_sum_exact
 *
 * Matches the PHP package's Complex::sumExact(): a + b exactly, as the rounded sum and its rounding error (Knuth's
 * two-sum). Relies on the build's -ffp-contract=off, like all the extension's float code.
 */
static void complex_sum_exact(double a, double b, double *sum, double *err)
{
	*sum = a + b;
	double b_virtual = *sum - a;
	*err = (a - (*sum - b_virtual)) + (b - b_virtual);
}
/* }}} */

/* {{{ complex_square_exact
 *
 * Matches the PHP package's Complex::squareExact(): x² exactly, as the rounded square and its rounding error, by
 * Dekker's product with Veltkamp's split. Valid when the square doesn't overflow.
 */
static void complex_square_exact(double x, double *square, double *err)
{
	*square = x * x;
	double c = 134217729.0 * x;
	double high = c - (c - x);
	double low = x - high;
	*err = ((high * high - *square) + 2 * high * low) + low * low;
}
/* }}} */

/* {{{ complex_calc_ln_magnitude
 *
 * Matches the PHP package's Complex::calcLnMagnitude(): ln|z|, accurate near |z| = 1. Between 0.5 and 2, it's
 * log1p(|z|² - 1) / 2, with |z|² - 1 = x² + y² - 1 calculated almost exactly, rather than log(|z|), which loses most
 * of its digits as |z| is rounded first.
 */
static double complex_calc_ln_magnitude(double real, double imag, double magnitude)
{
	if (magnitude <= 0.5 || magnitude >= 2) {
		return log(magnitude);
	}

	/* Add the terms from largest to smallest: the two squares, then -1, then the squares' rounding errors. */
	double x2, x2_err, y2, y2_err, sum, err1, err2, err3, err4;
	complex_square_exact(fmax(fabs(real), fabs(imag)), &x2, &x2_err);
	complex_square_exact(fmin(fabs(real), fabs(imag)), &y2, &y2_err);
	complex_sum_exact(x2, -1.0, &sum, &err1);
	complex_sum_exact(sum, y2, &sum, &err2);
	complex_sum_exact(sum, x2_err, &sum, &err3);
	complex_sum_exact(sum, y2_err, &sum, &err4);

	return log1p(sum + ((err1 + err2) + (err3 + err4))) / 2;
}
/* }}} */

/* {{{ complex_calc_ln
 *
 * Matches the PHP package's Complex::ln(): the natural logarithm. Uses the same shortcuts for
 * well-known values, falling back to ln(z) = ln|z| + i*arg(z) for the general case - reading the
 * magnitude/phase properties off obj (rather than recomputing hypot()/atan2() directly) so the
 * result matches the PHP package's calcLnMagnitude()/`$this->phase` exactly.
 *
 * Returns FAILURE (with ArithmeticException already thrown) if obj represents zero.
 */
zend_result complex_calc_ln(zend_object *obj, double *out_real, double *out_imag)
{
	double real, imag;
	complex_read_parts(obj, &real, &imag);

	if (real == 0.0 && imag == 0.0) {
		zend_throw_exception(math_ce_ArithmeticException, "Cannot compute logarithm of zero.", 0);
		return FAILURE;
	}

	if (imag == 0.0) {
		if (real == 1.0) {
			*out_real = 0.0;
			*out_imag = 0.0;
			return SUCCESS;
		}
		if (real == 2.0) {
			*out_real = M_LN2;
			*out_imag = 0.0;
			return SUCCESS;
		}
		if (real == M_E) {
			*out_real = 1.0;
			*out_imag = 0.0;
			return SUCCESS;
		}
		if (real == M_PI) {
			*out_real = log(M_PI);
			*out_imag = 0.0;
			return SUCCESS;
		}
		if (real == 10.0) {
			*out_real = M_LN10;
			*out_imag = 0.0;
			return SUCCESS;
		}
	}

	double magnitude, phase;
	complex_read_magnitude_phase(obj, &magnitude, &phase);

	*out_real = complex_calc_ln_magnitude(real, imag, magnitude);
	*out_imag = phase;
	return SUCCESS;
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::exp(): Complex */
PHP_METHOD(OceanMoon_Math_Complex, exp)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double real, imag, out_real, out_imag;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &real, &imag);
	if (complex_calc_exp(real, imag, &out_real, &out_imag) == FAILURE) {
		RETURN_THROWS();
	}

	if (complex_create(return_value, out_real, out_imag) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::ln(): Complex */
PHP_METHOD(OceanMoon_Math_Complex, ln)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double out_real, out_imag;
	if (complex_calc_ln(Z_OBJ_P(ZEND_THIS), &out_real, &out_imag) == FAILURE) {
		RETURN_THROWS();
	}

	if (complex_create(return_value, out_real, out_imag) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::log(Complex|float $base): Complex
 *
 * Matches the PHP package's Complex::log(): the logarithm with an arbitrary base, via the
 * change-of-base formula log_b(z) = ln(z) / ln(b). Unlike add()/sub()/mul()/div()/pow(), $base
 * needs to exist as a genuine Complex object (not just a (real, imaginary) pair) when it reaches
 * the general-solution branch, since complex_calc_ln() reads the magnitude/phase properties off a
 * real object - matching the PHP source, where a float $base is promoted via
 * `new self($base)` before `$base->ln()` is ever called. A promoted float therefore gets a
 * throwaway Complex allocated here, destroyed before returning.
 */
PHP_METHOD(OceanMoon_Math_Complex, log)
{
	zval *base_arg;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(base_arg)
	ZEND_PARSE_PARAMETERS_END();

	zval base_storage;
	bool base_is_temp = false;
	zend_object *base_obj;

	if (Z_TYPE_P(base_arg) == IS_OBJECT && instanceof_function(Z_OBJCE_P(base_arg), complex_ce_Complex)) {
		base_obj = Z_OBJ_P(base_arg);
	} else {
		double d;
		if (Z_TYPE_P(base_arg) == IS_LONG) {
			d = (double) Z_LVAL_P(base_arg);
		} else if (Z_TYPE_P(base_arg) == IS_DOUBLE) {
			d = Z_DVAL_P(base_arg);
		} else {
			zend_argument_type_error(
				1, "must be of type OceanMoon\\Math\\Complex|float, %s given", zend_zval_value_name(base_arg)
			);
			RETURN_THROWS();
		}
		if (!zend_finite(d)) {
			complex_throw_non_finite(d, 0.0);
			RETURN_THROWS();
		}
		if (complex_create(&base_storage, d, 0.0) == FAILURE) {
			RETURN_THROWS();
		}
		base_obj = Z_OBJ(base_storage);
		base_is_temp = true;
	}

	double base_real, base_imag;
	complex_read_parts(base_obj, &base_real, &base_imag);

	/* Check for invalid base values. */
	if (base_real == 0.0 && base_imag == 0.0) {
		if (base_is_temp) {
			zval_ptr_dtor(&base_storage);
		}
		zend_throw_exception(math_ce_ArithmeticException, "Cannot compute logarithm with base zero.", 0);
		RETURN_THROWS();
	}
	if (base_real == 1.0 && base_imag == 0.0) {
		if (base_is_temp) {
			zval_ptr_dtor(&base_storage);
		}
		zend_throw_exception(math_ce_ArithmeticException, "Cannot compute logarithm with base one.", 0);
		RETURN_THROWS();
	}

	double real, imag;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &real, &imag);

	/* Handle $this = 0. */
	if (real == 0.0 && imag == 0.0) {
		if (base_is_temp) {
			zval_ptr_dtor(&base_storage);
		}
		zend_throw_exception(math_ce_ArithmeticException, "Cannot compute logarithm of zero.", 0);
		RETURN_THROWS();
	}

	zend_result result;

	if (base_real == M_E && base_imag == 0.0) {
		/* Natural logarithm. */
		double ln_real, ln_imag;
		result = complex_calc_ln(Z_OBJ_P(ZEND_THIS), &ln_real, &ln_imag);
		if (result == SUCCESS) {
			result = complex_create(return_value, ln_real, ln_imag);
		}
	} else if (real == M_E && imag == 0.0 && base_real == 2.0 && base_imag == 0.0) {
		/* Built-in constant for log_2(e). */
		result = complex_create(return_value, M_LOG2E, 0.0);
	} else if (real == M_E && imag == 0.0 && base_real == 10.0 && base_imag == 0.0) {
		/* Built-in constant for log_10(e). */
		result = complex_create(return_value, M_LOG10E, 0.0);
	} else if (imag == 0.0 && base_imag == 0.0 && real > 0.0 && base_real > 0.0) {
		/* Positive reals only: log() returns the principal value only there, so a negative real $this or $base
		 * takes the general branch below. */
		/* Matches PHP's log($x, $base), which uses log2() or log10() for those bases. */
		double value = base_real == 2.0 ? log2(real) : base_real == 10.0 ? log10(real) : log(real) / log(base_real);
		result = complex_create(return_value, value, 0.0);
	} else {
		/* General solution: log_b(z) = ln(z) / ln(b). */
		double lnz_real, lnz_imag, lnb_real, lnb_imag;
		result = complex_calc_ln(Z_OBJ_P(ZEND_THIS), &lnz_real, &lnz_imag);
		if (result == SUCCESS) {
			result = complex_calc_ln(base_obj, &lnb_real, &lnb_imag);
		}
		/* ln(b) is non-zero, since base == 1 was rejected above. */
		double out_real, out_imag;
		if (result == SUCCESS) {
			result = complex_calc_div(lnz_real, lnz_imag, lnb_real, lnb_imag, &out_real, &out_imag);
			if (result == FAILURE) {
				math_rethrow_overflow("compute logarithm of the given Complex");
			}
		}
		if (result == SUCCESS) {
			result = complex_create(return_value, out_real, out_imag);
		}
	}

	if (base_is_temp) {
		zval_ptr_dtor(&base_storage);
	}

	if (result == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */
