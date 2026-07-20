/*
 * complex_transcendental.c
 *
 * Transcendental methods for OceanMoon\Math\Complex: exp(), ln(), and log(). Mirrors the
 * "Transcendental methods" region of the PHP package's Complex class.
 *
 * complex_calc_exp()/complex_calc_ln() are the computational cores, factored out so pow()
 * (complex_power.c) and log() (this file) can call them directly as pure functions -- reusing
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
#include "complex_arginfo.h"
#include "exceptions.h"

/* {{{ complex_calc_exp
 *
 * Matches the PHP package's Complex::exp(): e^z. Uses the same shortcuts (matching well-known
 * values exactly, including the two Eulerian identities), falling back to Euler's formula
 * (e^(a+bi) = e^a * (cos(b) + i*sin(b))) for the general case. Always succeeds -- e^z is finite
 * for every finite z, so there's no exception path here (unlike complex_calc_ln()).
 */
void complex_calc_exp(double real, double imag, double *out_real, double *out_imag)
{
	if (imag == 0.0) {
		if (real == 0.0) {
			*out_real = 1.0;
			*out_imag = 0.0;
			return;
		}
		if (real == 1.0) {
			*out_real = M_E;
			*out_imag = 0.0;
			return;
		}
		if (real == M_LN2) {
			*out_real = 2.0;
			*out_imag = 0.0;
			return;
		}
		if (real == log(M_PI)) {
			*out_real = M_PI;
			*out_imag = 0.0;
			return;
		}
		if (real == M_LN10) {
			*out_real = 10.0;
			*out_imag = 0.0;
			return;
		}
	} else if (real == 0.0) {
		/* Eulerian identities: e^(i*pi) = -1, e^(i*tau) = 1. */
		if (imag == M_PI) {
			*out_real = -1.0;
			*out_imag = 0.0;
			return;
		}
		if (imag == 2 * M_PI) {
			*out_real = 1.0;
			*out_imag = 0.0;
			return;
		}
	}

	/* General solution: Euler's formula. */
	*out_real = exp(real) * cos(imag);
	*out_imag = exp(real) * sin(imag);
}
/* }}} */

/* {{{ complex_calc_ln
 *
 * Matches the PHP package's Complex::ln(): the natural logarithm. Uses the same shortcuts for
 * well-known values, falling back to ln(z) = ln|z| + i*arg(z) for the general case -- reading the
 * magnitude/phase computed properties off obj (rather than recomputing hypot()/atan2() directly)
 * so the result matches the PHP package's `$mag = $this->magnitude; $phase = $this->phase;`
 * exactly, including the lazy caching.
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

	*out_real = log(magnitude);
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
	complex_calc_exp(real, imag, &out_real, &out_imag);

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
 * the general-solution branch, since complex_calc_ln() reads/caches the magnitude/phase computed
 * properties off a real object -- matching the PHP source, where a float $base is promoted via
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
			zend_argument_type_error(1, "must be of type Complex|float, %s given", zend_zval_type_name(base_arg));
			RETURN_THROWS();
		}
		if (!zend_finite(d)) {
			zend_throw_exception(spl_ce_DomainException, "Cannot create a complex number from non-finite values.", 0);
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
	} else if (imag == 0.0 && base_imag == 0.0) {
		/* Both arguments real: use the built-in two-argument log(). */
		result = complex_create(return_value, log(real) / log(base_real), 0.0);
	} else {
		/* General solution: log_b(z) = ln(z) / ln(b). */
		double lnz_real, lnz_imag, lnb_real, lnb_imag;
		result = complex_calc_ln(Z_OBJ_P(ZEND_THIS), &lnz_real, &lnz_imag);
		if (result == SUCCESS) {
			result = complex_calc_ln(base_obj, &lnb_real, &lnb_imag);
		}
		if (result == SUCCESS) {
			/* ln(z) is never zero here -- $this == 1 would have taken the "both real" branch above. */
			double f = (lnb_real * lnb_real) + (lnb_imag * lnb_imag);
			result = complex_create(
				return_value,
				(lnz_real * lnb_real + lnz_imag * lnb_imag) / f,
				(lnz_imag * lnb_real - lnz_real * lnb_imag) / f
			);
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
