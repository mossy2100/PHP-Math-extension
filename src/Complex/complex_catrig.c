/*
 * complex_catrig.c
 *
 * The computational core of the inverse trigonometric and hyperbolic methods of OceanMoon\Math\Complex: asin(),
 * acos(), atan(), asinh(), acosh() and atanh(). Matches the helpers calcAsinh() to calcLogLarge() in the PHP
 * package's Complex class, operation for operation, so the results agree bit for bit.
 *
 * Ported from FreeBSD's lib/msun/src/catrig.c, which implements the algorithms in T. E. Hull, T. F. Fairgrieve and
 * P. T. P. Tang, "Implementing the complex arcsine and arccosine functions using exception handling", ACM
 * Transactions on Mathematical Software 23(3), 1997, pages 299-335. They're accurate to within a few ulps for any
 * finite argument, and never overflow. The inputs are always finite here, so catrig.c's handling of INF and NAN is
 * left out. Its licence follows.
 *
 * Copyright (c) 2012 Stephen Montgomery-Smith <stephen@FreeBSD.ORG>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <float.h>
#include <math.h>
#include <stdbool.h>

#include "php.h"
#include "complex_internal.h"

/* Below this value of A, ln(A + √(A² - 1)) is calculated with log1p() (Hull et al. suggest 1.5; 10 works better). */
#define A_CROSSOVER 10.0

/* Above this value of B, asin(B) is calculated with atan2(), as suggested by Hull et al. */
#define B_CROSSOVER 0.6417

/* At least 4·√DBL_MIN: 2^-509. Decimal, as MSVC may not accept hex float literals. */
#define FOUR_SQRT_MIN 5.966672584960166e-154

/* At most √DBL_MAX / 4: 2^509. */
#define QUARTER_SQRT_MAX 1.6759759912428246e+153

/* At least √DBL_MIN: 2^-511. */
#define SQRT_MIN 1.4916681462400413e-154

/* Above this magnitude, a part is so large that the asymptotic formulas are exact to within rounding. */
#define RECIP_EPSILON (1 / DBL_EPSILON)

/* √(3·DBL_EPSILON) and √(6·DBL_EPSILON). */
#define SQRT_3_EPSILON 2.5809568279517849e-8
#define SQRT_6_EPSILON 3.6500241499888571e-8

/* π/2, and its rounding error. */
#define PI_2_HI 1.5707963267948966
#define PI_2_LO 6.1232339957367659e-17

/* ln 2 and e, as PHP's M_LN2 and M_E. */
#define LN_2 0.69314718055994531
#define E 2.7182818284590452

/* {{{ calc_half_hypot_diff
 *
 * (hypot(a, b) - b) / 2, without cancellation, given hypot(a, b). f() in catrig.c, and calcHalfHypotDiff() in the
 * package.
 */
static double calc_half_hypot_diff(double a, double b, double hypot_a_b)
{
	if (b < 0) {
		return (hypot_a_b - b) / 2;
	}
	if (b == 0) {
		return a / 2;
	}
	/* hypot(a, b) - b = a² / (hypot(a, b) + b), which doesn't cancel when b is positive. */
	return a * a / (hypot_a_b + b) / 2;
}
/* }}} */

/* {{{ calc_asinh_parts
 *
 * The parts of asinh(x + iy), where x and y are non-negative and less than RECIP_EPSILON. do_hard_work() in
 * catrig.c, and calcAsinhParts() in the package, which explains the method.
 *
 * Sets *rx to the real part of asinh(z), and returns whether B = y / A is usable. If it is, *b is B. If not,
 * *sqrt_a2my2 is √(A² - y²) and *new_y is y, both possibly scaled up by the same factor to avoid underflow.
 */
static bool calc_asinh_parts(double x, double y, double *rx, double *b, double *sqrt_a2my2, double *new_y)
{
	/* |z + i| and |z - i|. */
	double r = hypot(x, y + 1);
	double s = hypot(x, y - 1);

	/* A is at least 1 mathematically, but rounding could take it just below. */
	double a = (r + s) / 2;
	if (a < 1.0) {
		a = 1.0;
	}

	/* Calculate the real part, ln(A + √(A² - 1)). */
	if (a < A_CROSSOVER) {
		if (y == 1.0 && x < DBL_EPSILON * DBL_EPSILON / 128) {
			/* A - 1 is about x / 2. */
			*rx = sqrt(x);
		} else if (x >= DBL_EPSILON * fabs(y - 1)) {
			double a_minus_1 = calc_half_hypot_diff(x, 1 + y, r) + calc_half_hypot_diff(x, 1 - y, s);
			*rx = log1p(a_minus_1 + sqrt(a_minus_1 * (a + 1)));
		} else if (y < 1) {
			/* A is 1, to within rounding. */
			*rx = x / sqrt((1 - y) * (1 + y));
		} else {
			/* A - 1 is y - 1, to within rounding. */
			*rx = log1p((y - 1) + sqrt((y - 1) * (y + 1)));
		}
	} else {
		*rx = log(a + sqrt(a * a - 1));
	}

	*new_y = y;

	/* For a tiny y, y / A could underflow, so return √(A² - y²) and y, scaled up, for atan2(). */
	if (y < FOUR_SQRT_MIN) {
		*b = 0.0;
		*sqrt_a2my2 = a * (2 / DBL_EPSILON);
		*new_y = y * (2 / DBL_EPSILON);
		return false;
	}

	/* B = y / A, which is usable unless it's close to 1, where asin() loses accuracy. */
	*b = y / a;
	if (*b <= B_CROSSOVER) {
		*sqrt_a2my2 = 0.0;
		return true;
	}

	/* Calculate √(A² - y²) for atan2(). */
	if (y == 1.0 && x < DBL_EPSILON / 128) {
		/* A - y is about x / 2. */
		*sqrt_a2my2 = sqrt(x) * sqrt((a + y) / 2);
	} else if (x >= DBL_EPSILON * fabs(y - 1)) {
		double a_minus_y = calc_half_hypot_diff(x, y + 1, r) + calc_half_hypot_diff(x, y - 1, s);
		*sqrt_a2my2 = sqrt(a_minus_y * (a + y));
	} else if (y > 1) {
		/* A is y, to within rounding. Scale up to avoid underflow; y < RECIP_EPSILON, so this can't overflow. */
		double scale = 4 / DBL_EPSILON / DBL_EPSILON;
		*sqrt_a2my2 = x * scale * y / sqrt((y + 1) * (y - 1));
		*new_y = y * scale;
	} else {
		/* A is 1, to within rounding. */
		*sqrt_a2my2 = sqrt((1 - y) * (1 + y));
	}

	return false;
}
/* }}} */

/* {{{ calc_log_large
 *
 * ln(x + iy) for a large x + iy, without overflowing. clog_for_large_values() in catrig.c, and calcLogLarge() in the
 * package.
 */
static void calc_log_large(double x, double y, double *out_re, double *out_im)
{
	double max = fmax(fabs(x), fabs(y));
	double min = fmin(fabs(x), fabs(y));

	*out_im = atan2(y, x);

	/* hypot() could overflow, so divide both parts by e, and add 1 to the logarithm. */
	if (max > DBL_MAX / 2) {
		*out_re = log(hypot(x / E, y / E)) + 1;
	} else if (max > QUARTER_SQRT_MAX || min < SQRT_MIN) {
		/* Squaring could overflow or underflow. */
		*out_re = log(hypot(x, y));
	} else {
		*out_re = log(max * max + min * min) / 2;
	}
}
/* }}} */

/* {{{ complex_calc_asinh
 *
 * asinh(x + iy), for asinh() and asin(). calcAsinh() in the package.
 */
void complex_calc_asinh(double x, double y, double *out_re, double *out_im)
{
	double ax = fabs(x);
	double ay = fabs(y);

	/* For a large argument, asinh(z) = sign(x)·(ln(sign(x)·z) + ln 2), to within rounding. */
	if (ax > RECIP_EPSILON || ay > RECIP_EPSILON) {
		double log_re, log_im;
		if (signbit(x)) {
			calc_log_large(-x, -y, &log_re, &log_im);
		} else {
			calc_log_large(x, y, &log_re, &log_im);
		}
		*out_re = copysign(log_re + LN_2, x);
		*out_im = copysign(log_im, y);
		return;
	}

	/* For a small argument (including zero), asinh(z) = z, to within rounding. */
	if (ax < SQRT_6_EPSILON / 4 && ay < SQRT_6_EPSILON / 4) {
		*out_re = x;
		*out_im = y;
		return;
	}

	double rx, b, sqrt_a2my2, new_y;
	double ry = calc_asinh_parts(ax, ay, &rx, &b, &sqrt_a2my2, &new_y) ? asin(b) : atan2(new_y, sqrt_a2my2);
	*out_re = copysign(rx, x);
	*out_im = copysign(ry, y);
}
/* }}} */

/* {{{ complex_calc_acos
 *
 * acos(x + iy), for acos() and acosh(). calcAcos() in the package.
 */
void complex_calc_acos(double x, double y, double *out_re, double *out_im)
{
	double ax = fabs(x);
	double ay = fabs(y);

	/* For a large argument, acos(z) = -sign(y)·i·(ln(z) + ln 2), to within rounding. */
	if (ax > RECIP_EPSILON || ay > RECIP_EPSILON) {
		double log_re, log_im;
		calc_log_large(x, y, &log_re, &log_im);
		double ry = log_re + LN_2;
		*out_re = fabs(log_im);
		*out_im = signbit(y) ? ry : -ry;
		return;
	}

	/* acos(1) = 0 exactly. */
	if (x == 1.0 && y == 0.0) {
		*out_re = 0.0;
		*out_im = -y;
		return;
	}

	/* For a small argument, acos(z) = π/2 - z, to within rounding. */
	if (ax < SQRT_6_EPSILON / 4 && ay < SQRT_6_EPSILON / 4) {
		*out_re = PI_2_HI - (x - PI_2_LO);
		*out_im = -y;
		return;
	}

	/* The same calculation as asinh(), with the parts swapped. */
	double ry, b, sqrt_a2mx2, new_x;
	if (calc_asinh_parts(ay, ax, &ry, &b, &sqrt_a2mx2, &new_x)) {
		*out_re = acos(signbit(x) ? -b : b);
	} else {
		*out_re = atan2(sqrt_a2mx2, signbit(x) ? -new_x : new_x);
	}
	*out_im = signbit(y) ? ry : -ry;
}
/* }}} */

/* {{{ calc_real_part_reciprocal
 *
 * The real part of 1 / (x + iy), x / (x² + y²), for a large x + iy, without overflowing or underflowing needlessly.
 * calcRealPartReciprocal() in the package.
 */
static double calc_real_part_reciprocal(double x, double y)
{
	/* Scale so the larger part is 1, so the squares can't overflow. */
	double scale = fmax(fabs(x), fabs(y));
	x /= scale;
	y /= scale;
	return x / (x * x + y * y) / scale;
}
/* }}} */

/* {{{ complex_calc_atanh
 *
 * atanh(x + iy), for atanh() and atan(). calcAtanh() in the package. If y is 0, x must not be ±1.
 */
void complex_calc_atanh(double x, double y, double *out_re, double *out_im)
{
	double ax = fabs(x);
	double ay = fabs(y);

	/* A real argument in [-1, 1]. */
	if (y == 0.0 && ax <= 1) {
		*out_re = atanh(x);
		*out_im = y;
		return;
	}

	/* An imaginary argument, matching the accuracy of atan(). */
	if (x == 0.0) {
		*out_re = x;
		*out_im = atan(y);
		return;
	}

	/* For a large argument, atanh(z) = 1/z + sign(y)·iπ/2, to within rounding. */
	if (ax > RECIP_EPSILON || ay > RECIP_EPSILON) {
		*out_re = calc_real_part_reciprocal(x, y);
		*out_im = copysign(M_PI_2, y);
		return;
	}

	/* For a small argument, atanh(z) = z, to within rounding. */
	if (ax < SQRT_3_EPSILON / 2 && ay < SQRT_3_EPSILON / 2) {
		*out_re = x;
		*out_im = y;
		return;
	}

	/* Real part. Near ±1, log1p(4/|z - 1|²) is ln(4/y²) = ln 2 - 2·ln(y), which avoids squaring a tiny y. */
	double rx;
	if (ax == 1.0 && ay < DBL_EPSILON) {
		rx = (LN_2 - log(ay)) / 2;
	} else {
		double ax1 = ax - 1;
		double sum_squares = ay < SQRT_MIN ? ax1 * ax1 : ax1 * ax1 + ay * ay;
		rx = log1p(4 * ax / sum_squares) / 4;
	}

	/* Imaginary part. */
	double ry;
	if (ax == 1.0) {
		ry = atan2(2, -ay) / 2;
	} else if (ay < DBL_EPSILON) {
		ry = atan2(2 * ay, (1 - ax) * (1 + ax)) / 2;
	} else {
		ry = atan2(2 * ay, (1 - ax) * (1 + ax) - ay * ay) / 2;
	}

	*out_re = copysign(rx, x);
	*out_im = copysign(ry, y);
}
/* }}} */
