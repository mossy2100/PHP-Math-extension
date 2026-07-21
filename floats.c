/*
 * floats.c
 *
 * See floats.h.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <math.h>

#include "floats.h"

bool math_floats_approx_equal(double a, double b, double rel_tol, double abs_tol)
{
	/* NaN is never approximately equal to anything, including itself. */
	if (isnan(a) || isnan(b)) {
		return false;
	}

	/* Tolerance math doesn't apply to infinities -- fall back to exact equality. */
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

double math_floats_normalize_zero(double value)
{
	return value == 0.0 ? 0.0 : value;
}

double math_floats_wrap(double value, double units_per_turn, bool signed_range)
{
	/* Reduce using fmod to avoid large magnitudes. r is in [0, units_per_turn) if value is
	 * positive, or (-units_per_turn, 0] if negative. */
	double r = fmod(value, units_per_turn);

	if (signed_range) {
		/* Signed range is (-half, half]. */
		double half = units_per_turn / 2.0;
		if (r <= -half) {
			r += units_per_turn;
		} else if (r > half) {
			r -= units_per_turn;
		}
	} else {
		/* Unsigned range is [0, units_per_turn). */
		if (r < 0.0) {
			r += units_per_turn;
		}
	}

	return math_floats_normalize_zero(r);
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
	return Z_STR(tmp);
}
