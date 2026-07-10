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
