/*
 * integers.c
 *
 * See integers.h.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "integers.h"

/* The checks below are portable, rather than GCC/Clang's __builtin_*_overflow(), which MSVC doesn't have. Each
 * checks the operands before the operation, so no signed overflow ever happens. */

math_integers_result math_integers_add(zend_long a, zend_long b, zend_long *out)
{
	if ((b > 0 && a > ZEND_LONG_MAX - b) || (b < 0 && a < ZEND_LONG_MIN - b)) {
		return MATH_INTEGERS_OVERFLOW;
	}

	*out = a + b;
	return MATH_INTEGERS_OK;
}

math_integers_result math_integers_sub(zend_long a, zend_long b, zend_long *out)
{
	if ((b < 0 && a > ZEND_LONG_MAX + b) || (b > 0 && a < ZEND_LONG_MIN + b)) {
		return MATH_INTEGERS_OVERFLOW;
	}

	*out = a - b;
	return MATH_INTEGERS_OK;
}

math_integers_result math_integers_mul(zend_long a, zend_long b, zend_long *out)
{
	/* The engine's own overflow-checked multiply, as used by PHP's *, which picks the best method for the compiler
	 * and platform. */
	zend_long c;
	double unused;
	int overflowed;
	ZEND_SIGNED_MULTIPLY_LONG(a, b, c, unused, overflowed);
	(void) unused;

	if (overflowed) {
		return MATH_INTEGERS_OVERFLOW;
	}

	*out = c;
	return MATH_INTEGERS_OK;
}

/* Ports OceanMoon\Core\Integers::addModNegative(): (x + y) reduced into (neg_m, 0], for x and y in (neg_m, 0],
 * without overflowing. Used by math_integers_mul_mod(). */
static zend_long math_integers_add_mod_negative(zend_long x, zend_long y, zend_long neg_m)
{
	/* If x + y would reach neg_m or below, subtract neg_m. Checking against neg_m - y, which is in [neg_m, 0), means
	 * x + y is never calculated when it could overflow. */
	return x <= neg_m - y ? x - (neg_m - y) : x + y;
}

math_integers_result math_integers_mul_mod(zend_long a, zend_long b, zend_long m, zend_long *out)
{
	if (m == 0) {
		return MATH_INTEGERS_DIVISION_BY_ZERO;
	}

	/* Any integer is a multiple of 1 and -1. Handled first, since ZEND_LONG_MIN % -1 overflows in C, though PHP gives
	 * 0. */
	if (m == 1 || m == -1) {
		*out = 0;
		return MATH_INTEGERS_OK;
	}

	/* If the product fits in a zend_long, use it directly. */
	zend_long product;
	if (math_integers_mul(a, b, &product) == MATH_INTEGERS_OK) {
		*out = product % m;
		return MATH_INTEGERS_OK;
	}

	/* Work with non-positive values, which can represent every magnitude up to |ZEND_LONG_MIN| = 2^63, so m can be
	 * ZEND_LONG_MIN. Each value is the negative of a magnitude in [0, |m|). */
	zend_long neg_m = m > 0 ? -m : m;
	zend_long x = a % m;
	x = x > 0 ? -x : x;

	/* |b % m| is less than |m|, so at most 2^63 - 1, and can be made positive. */
	zend_long y = b % m;
	y = y < 0 ? -y : y;

	/* Add up x times each set bit of y, doubling x at each step. */
	zend_long result = 0;
	while (y > 0) {
		if ((y & 1) == 1) {
			result = math_integers_add_mod_negative(result, x, neg_m);
		}
		x = math_integers_add_mod_negative(x, x, neg_m);
		y >>= 1;
	}

	/* The result is -(|a * b| % |m|). Give it the sign of the product. It's greater than -2^63, so negating it can't
	 * overflow. */
	*out = (a < 0) == (b < 0) ? -result : result;
	return MATH_INTEGERS_OK;
}

math_integers_result math_integers_pow(zend_long a, zend_long b, zend_long *out)
{
	if (b < 0) {
		return MATH_INTEGERS_NEGATIVE_EXPONENT;
	}

	/* Exponentiation by squaring, checking for overflow at every multiplication - there's no
	 * single overflow-checked "pow" builtin to call once, so the check has to happen at each step
	 * the way the naive a**b computation would eventually overflow at the same magnitude. */
	zend_long result = 1;
	zend_long base = a;
	zend_long exponent = b;

	while (exponent > 0) {
		if (exponent % 2 == 1) {
			if (math_integers_mul(result, base, &result) != MATH_INTEGERS_OK) {
				return MATH_INTEGERS_OVERFLOW;
			}
		}

		exponent /= 2;

		/* Skip the final, unused squaring of `base` once the loop is about to end - squaring a
		 * large base one step too many can overflow even when the true result doesn't need it. */
		if (exponent > 0) {
			if (math_integers_mul(base, base, &base) != MATH_INTEGERS_OK) {
				return MATH_INTEGERS_OVERFLOW;
			}
		}
	}

	*out = result;
	return MATH_INTEGERS_OK;
}

math_integers_result math_integers_gcd(const zend_long *nums, size_t count, zend_long *out)
{
	if (count == 0) {
		return MATH_INTEGERS_EMPTY;
	}

	/* Run Euclid's algorithm on the raw (signed) values, without calling abs() on any
	 * intermediate value. A remainder's magnitude is always smaller than the divisor's, so it
	 * can never itself be ZEND_LONG_MIN - abs() is only ever needed once, on the final result. */
	zend_long result = nums[0];

	for (size_t i = 1; i < count; i++) {
		zend_long b = nums[i];

		while (b != 0) {
			zend_long temp = b;
			/* result % -1 is always 0, but evaluating that expression directly is undefined
			 * behavior in C when result == ZEND_LONG_MIN (INT_MIN % -1 overflows, unlike PHP's %
			 * operator, which handles it safely) - special-case it instead. */
			b = (b == -1) ? 0 : result % b;
			result = temp;
		}
	}

	/* The only way the final result can still be ZEND_LONG_MIN is if nothing above ever reduced
	 * it, i.e. every other argument was 0 or ZEND_LONG_MIN itself. In that case the true GCD is
	 * ZEND_LONG_MIN's magnitude, which doesn't fit in a zend_long. */
	if (result == ZEND_LONG_MIN) {
		return MATH_INTEGERS_OVERFLOW;
	}

	*out = result < 0 ? -result : result;
	return MATH_INTEGERS_OK;
}
