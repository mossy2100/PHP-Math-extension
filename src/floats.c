/*
 * floats.c
 *
 * See floats.h.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <math.h>

#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "floats.h"

bool math_floats_approx_equal(double a, double b, double rel_tol, double abs_tol)
{
	/* NaN is never approximately equal to anything, including itself. */
	if (isnan(a) || isnan(b)) {
		return false;
	}

	/* Tolerance math doesn't apply to infinities - fall back to exact equality. */
	if (isinf(a) || isinf(b)) {
		return a == b;
	}

	/* Exact equality (handles identical values). */
	if (a == b) {
		return true;
	}

	double diff = fabs(a - b);

	/* Absolute tolerance, useful for values near zero. */
	if (diff <= abs_tol) {
		return true;
	}

	/* Relative tolerance otherwise. */
	return diff <= rel_tol * fmax(fabs(a), fabs(b));
}

/* Throws DomainException for an invalid tolerance, naming it in the message. */
static zend_result math_floats_validate_tolerance(const char *name, double tol)
{
	if (zend_finite(tol) && tol >= 0.0) {
		return SUCCESS;
	}

	zend_string *tol_str = math_floats_to_str(tol);
	zend_string *msg = strpprintf(
		0, "Invalid %s tolerance: %s. Must be finite and non-negative.", name, ZSTR_VAL(tol_str));
	zend_throw_exception(spl_ce_DomainException, ZSTR_VAL(msg), 0);
	zend_string_release(msg);
	zend_string_release(tol_str);
	return FAILURE;
}

zend_result math_floats_validate_tolerances(double rel_tol, double abs_tol)
{
	if (math_floats_validate_tolerance("relative", rel_tol) == FAILURE) {
		return FAILURE;
	}
	return math_floats_validate_tolerance("absolute", abs_tol);
}

double math_floats_normalize_zero(double value)
{
	return value == 0.0 ? 0.0 : value;
}

bool math_floats_is_int(double value)
{
	return isfinite(value) && floor(value) == value;
}

bool math_floats_to_int(double f, zend_long *out)
{
	/* PHP_INT_MIN is exactly representable as a double (it's a power of two); PHP_INT_MAX is not.
	 * See the doc comment in floats.h for why the upper bound is `f < -limit` rather than
	 * `f <= (double) ZEND_LONG_MAX`. */
	double limit = (double) ZEND_LONG_MIN;
	if (math_floats_is_int(f) && f >= limit && f < -limit) {
		*out = (zend_long) f;
		return true;
	}

	return false;
}

zend_string *math_floats_to_str(double value)
{
	if (zend_isnan(value)) {
		return zend_string_init("NAN", sizeof("NAN") - 1, 0);
	}
	if (zend_isinf(value)) {
		return value > 0
			? zend_string_init("INF", sizeof("INF") - 1, 0)
			: zend_string_init("-INF", sizeof("-INF") - 1, 0);
	}

	zval tmp;
	ZVAL_DOUBLE(&tmp, value);
	convert_to_string(&tmp);
	zend_string *str = Z_STR(tmp);

	/* If nothing distinguishes the string from an int (no decimal point or exponent), append
	 * ".0", matching stringifyFloat(). */
	if (strpbrk(ZSTR_VAL(str), ".eE") == NULL) {
		zend_string *with_suffix = zend_string_concat2(ZSTR_VAL(str), ZSTR_LEN(str), ".0", 2);
		zend_string_release(str);
		return with_suffix;
	}

	return str;
}

double math_floats_power_of_2_scale(double max_abs)
{
	if (max_abs == 0.0) {
		return 0.0;
	}

	/* log2() can round up to the next power of 2, e.g. to 1024 near DBL_MAX, where exp2(1024) is INF. If so, use the
	 * power below. */
	double exponent = floor(log2(max_abs));
	if (exp2(exponent) > max_abs) {
		exponent -= 1;
	}
	return exp2(exponent);
}
