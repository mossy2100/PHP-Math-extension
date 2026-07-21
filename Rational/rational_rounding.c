/*
 * rational_rounding.c
 *
 * Rounding methods for OceanMoon\Math\Rational: round(), floor(), ceil(). Mirrors the "Rounding
 * methods" region of the PHP package's Rational class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/php_math_round_mode.h"
#include "rational_internal.h"
#include "rational_arginfo.h"

/* {{{ OceanMoon\Math\Rational::round(RoundingMode $mode = RoundingMode::HalfAwayFromZero): int
 *
 * Matches the PHP package's Rational::round() exactly: all arithmetic is performed exactly on the
 * numerator and denominator, never converting to a float, so there's no precision loss.
 *
 * php_math_round_mode_from_enum() (used by Complex::round() too) translates the RoundingMode enum
 * case into one of PHP's eight distinct legacy PHP_ROUND_* ints (ext/standard/php_math_round_mode.h)
 * -- unlike Complex's use of it, which only needs to distinguish tie-breaking behavior for a
 * single "round to nearest" implementation, this switches on all eight directly, since Rational's
 * own round() implements NegativeInfinity/PositiveInfinity/TowardsZero/AwayFromZero natively
 * rather than delegating to PHP's built-in round().
 */
PHP_METHOD(OceanMoon_Math_Rational, round)
{
	zend_object *mode_obj = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJ_OF_CLASS_OR_NULL(mode_obj, rounding_mode_ce)
	ZEND_PARSE_PARAMETERS_END();

	int mode = mode_obj ? php_math_round_mode_from_enum(mode_obj) : PHP_ROUND_HALF_UP;

	zend_long num, den;
	rational_read_parts(Z_OBJ_P(ZEND_THIS), &num, &den);

	if (den == 1) {
		RETURN_LONG(num);
	}

	/* Since the Rational is always in lowest terms and the denominator isn't 1 (checked above),
	 * the denominator cannot evenly divide the numerator, so the remainder is guaranteed to be
	 * non-zero. */
	zend_long q = num / den;
	zend_long r = num % den;
	zend_long abs_r = r < 0 ? -r : r;
	/* +/-1 can't overflow: |q| <= |num|/2 (den >= 2 here), comfortably below ZEND_LONG_MAX. */
	zend_long away = num > 0 ? q + 1 : q - 1;

	switch (mode) {
		case PHP_ROUND_TOWARD_ZERO:
			/* Always truncate toward zero, ignoring the remainder entirely. */
			RETURN_LONG(q);

		case PHP_ROUND_AWAY_FROM_ZERO:
			/* Always round away from zero, since there's always a non-zero remainder here. */
			RETURN_LONG(away);

		case PHP_ROUND_FLOOR:
			/* Round down (toward negative infinity). Truncation already does this for a positive
			 * value; a negative value needs its magnitude increased by one. */
			RETURN_LONG(num > 0 ? q : away);

		case PHP_ROUND_CEILING:
			/* Round up (toward positive infinity). Mirror image of FLOOR. */
			RETURN_LONG(num > 0 ? away : q);

		case PHP_ROUND_HALF_UP:
			/* Round to the nearest integer; an exact tie (remainder exactly half the
			 * denominator) rounds away from zero. Comparing abs_r against (den - abs_r) is
			 * equivalent to comparing 2 * abs_r against den, but avoids the doubling ever
			 * overflowing for a remainder near ZEND_LONG_MAX. */
			RETURN_LONG(abs_r >= den - abs_r ? away : q);

		case PHP_ROUND_HALF_DOWN:
			/* Round to the nearest integer; an exact tie rounds toward zero. */
			RETURN_LONG(abs_r > den - abs_r ? away : q);

		case PHP_ROUND_HALF_EVEN:
			/* Round to the nearest integer; an exact tie rounds to whichever of q/away is even
			 * ("banker's rounding"). q and away always differ by exactly 1, so exactly one of
			 * them is even. */
			if (abs_r > den - abs_r) {
				RETURN_LONG(away);
			}
			if (abs_r < den - abs_r) {
				RETURN_LONG(q);
			}
			RETURN_LONG(q % 2 == 0 ? q : away);

		case PHP_ROUND_HALF_ODD:
			/* Round to the nearest integer; an exact tie rounds to whichever of q/away is odd. */
			if (abs_r > den - abs_r) {
				RETURN_LONG(away);
			}
			if (abs_r < den - abs_r) {
				RETURN_LONG(q);
			}
			RETURN_LONG(q % 2 != 0 ? q : away);

		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::floor(): int
 *
 * Matches the PHP package's Rational::floor().
 */
PHP_METHOD(OceanMoon_Math_Rational, floor)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_long num, den;
	rational_read_parts(Z_OBJ_P(ZEND_THIS), &num, &den);

	if (den == 1) {
		RETURN_LONG(num);
	}

	/* C's `/` truncates toward zero, so for negative fractions the quotient is already rounded
	 * up (toward zero). We need to subtract 1 to floor it (toward negative infinity). For
	 * positive fractions, truncation already gives the floor. */
	zend_long q = num / den;
	RETURN_LONG(num < 0 ? q - 1 : q);
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::ceil(): int
 *
 * Matches the PHP package's Rational::ceil().
 */
PHP_METHOD(OceanMoon_Math_Rational, ceil)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_long num, den;
	rational_read_parts(Z_OBJ_P(ZEND_THIS), &num, &den);

	if (den == 1) {
		RETURN_LONG(num);
	}

	/* C's `/` truncates toward zero, so for positive fractions the quotient is already rounded
	 * down (toward zero). We need to add 1 to ceil it (toward positive infinity). For negative
	 * fractions, truncation already gives the ceiling. */
	zend_long q = num / den;
	RETURN_LONG(num > 0 ? q + 1 : q);
}
/* }}} */
