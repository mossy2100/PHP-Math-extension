/*
 * integers.c
 *
 * See integers.h.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "integers.h"

math_integers_result math_integers_add(zend_long a, zend_long b, zend_long *out)
{
	zend_long c;

	if (__builtin_add_overflow(a, b, &c)) {
		return MATH_INTEGERS_OVERFLOW;
	}

	*out = c;
	return MATH_INTEGERS_OK;
}

math_integers_result math_integers_mul(zend_long a, zend_long b, zend_long *out)
{
	zend_long c;

	if (__builtin_mul_overflow(a, b, &c)) {
		return MATH_INTEGERS_OVERFLOW;
	}

	*out = c;
	return MATH_INTEGERS_OK;
}

math_integers_result math_integers_pow(zend_long a, zend_long b, zend_long *out)
{
	if (b < 0) {
		return MATH_INTEGERS_NEGATIVE_EXPONENT;
	}

	/* Exponentiation by squaring, checking for overflow at every multiplication -- there's no
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

		/* Skip the final, unused squaring of `base` once the loop is about to end -- squaring a
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

	if (nums[0] == ZEND_LONG_MIN) {
		return MATH_INTEGERS_MIN_LONG;
	}

	zend_long result = nums[0] < 0 ? -nums[0] : nums[0];

	for (size_t i = 1; i < count; i++) {
		if (nums[i] == ZEND_LONG_MIN) {
			return MATH_INTEGERS_MIN_LONG;
		}

		zend_long a = result;
		zend_long b = nums[i] < 0 ? -nums[i] : nums[i];

		while (b != 0) {
			zend_long temp = b;
			b = a % b;
			a = temp;
		}

		result = a;
	}

	*out = result;
	return MATH_INTEGERS_OK;
}
