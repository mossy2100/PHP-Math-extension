/*
 * rational_power.c
 *
 * Power methods for OceanMoon\Math\Rational: pow(), sqr(). Mirrors the "Power methods" region of
 * the PHP package's Rational class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "rational_internal.h"
#include "rational_arginfo.h"
#include "exceptions.h"
#include "integers.h"

/* {{{ rational_pow_checked
 *
 * Thin wrapper around math_integers_pow() that throws with the exact messages
 * Core\Integers::pow() itself uses, matching the PHP package's pow() (which calls Integers::pow()
 * directly and lets its exceptions propagate). The negative-exponent case can't actually be
 * reached from rational_calc_pow() below (every negative-exponent path is handled before the
 * general-solution call site that uses this), but it's guarded anyway rather than assuming.
 */
static zend_result rational_pow_checked(zend_long a, zend_long b, zend_long *out)
{
	math_integers_result result = math_integers_pow(a, b, out);

	if (result == MATH_INTEGERS_OK) {
		return SUCCESS;
	}

	if (result == MATH_INTEGERS_NEGATIVE_EXPONENT) {
		zend_string *msg = strpprintf(0, "Invalid exponent: " ZEND_LONG_FMT ". Must not be negative.", b);
		zend_throw_exception(spl_ce_DomainException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	zend_throw_exception(spl_ce_OverflowException, "Overflow in integer exponentiation.", 0);
	return FAILURE;
}
/* }}} */

/* {{{ rational_calc_pow
 *
 * The computational core of pow(), factored out so it can recurse on itself (matching the PHP
 * source's own `$this->pow(...)` recursive calls) without going through a full PHP method
 * dispatch each time.
 *
 * Matches the PHP package's Rational::pow() exactly, including its shortcuts (exponent 0, base 0,
 * exponent 1/2/-1, exponent ZEND_LONG_MIN) -- the exponent-2 and exponent-ZEND_LONG_MIN branches
 * reuse rational_calc_mul()/rational_calc_inv() (rational_arithmetic.c) directly, same as the PHP
 * source's sqr()/mul()/inv() delegations.
 */
static zend_result rational_calc_pow(zend_long num, zend_long den, zend_long exponent, zval *return_value)
{
	/* Any number to the power of 0 is 1, including 0 (by convention). */
	if (exponent == 0) {
		return rational_create(return_value, 1, 1);
	}

	/* Handle 0 base. */
	if (num == 0) {
		if (exponent < 0) {
			zend_throw_exception(math_ce_ArithmeticException, "Cannot raise zero to negative power.", 0);
			return FAILURE;
		}
		return rational_create(return_value, 0, 1);
	}

	/* Exponent 1: any number to power 1 is itself -- a new, distinct instance (matching the PHP
	 * source's `clone $this`), not literally the same object. */
	if (exponent == 1) {
		return rational_create(return_value, num, den);
	}

	/* Exponent 2: delegate to sqr() (mul(this, this) -- reuses rational_calc_mul() directly). */
	if (exponent == 2) {
		return rational_calc_mul(num, den, num, den, return_value);
	}

	/* Exponent -1: delegate to inv(). Safe -- base != 0 already established above. */
	if (exponent == -1) {
		return rational_calc_inv(num, den, return_value);
	}

	/* Exponent ZEND_LONG_MIN: negating it directly (as the general negative-exponent branch
	 * below does) would overflow, so it's handled via pow(ZEND_LONG_MAX)->mul(this)->inv()
	 * instead, matching the PHP source's own special case for exactly this reason. */
	if (exponent == ZEND_LONG_MIN) {
		zval tmp;
		if (rational_calc_pow(num, den, ZEND_LONG_MAX, &tmp) == FAILURE) {
			return FAILURE;
		}
		zend_long tmp_num, tmp_den;
		rational_read_parts(Z_OBJ(tmp), &tmp_num, &tmp_den);
		zval_ptr_dtor(&tmp);

		zval mul_result;
		if (rational_calc_mul(tmp_num, tmp_den, num, den, &mul_result) == FAILURE) {
			return FAILURE;
		}
		zend_long mul_num, mul_den;
		rational_read_parts(Z_OBJ(mul_result), &mul_num, &mul_den);
		zval_ptr_dtor(&mul_result);

		return rational_calc_inv(mul_num, mul_den, return_value);
	}

	/* Handle negative exponents by taking the reciprocal. -exponent is always safe here:
	 * exponent < 0 and exponent != ZEND_LONG_MIN (handled above). */
	if (exponent < 0) {
		zval inv;
		if (rational_calc_inv(num, den, &inv) == FAILURE) {
			return FAILURE;
		}
		zend_long inv_num, inv_den;
		rational_read_parts(Z_OBJ(inv), &inv_num, &inv_den);
		zend_result result = rational_calc_pow(inv_num, inv_den, -exponent, return_value);
		zval_ptr_dtor(&inv);
		return result;
	}

	/* General solution. Calculate the new numerator and denominator with overflow checks. */
	zend_long h, k;
	if (rational_pow_checked(num, exponent, &h) == FAILURE || rational_pow_checked(den, exponent, &k) == FAILURE) {
		return FAILURE;
	}

	return rational_create(return_value, h, k);
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::pow(int $exponent): Rational */
PHP_METHOD(OceanMoon_Math_Rational, pow)
{
	zend_long exponent;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(exponent)
	ZEND_PARSE_PARAMETERS_END();

	zend_long num, den;
	rational_read_parts(Z_OBJ_P(ZEND_THIS), &num, &den);

	if (rational_calc_pow(num, den, exponent, return_value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::sqr(): Rational
 *
 * Matches the PHP package's Rational::sqr(): this * this. Equivalent to pow(2), but more
 * efficient/readable -- reuses rational_calc_mul() directly, same as rational_calc_pow()'s own
 * exponent-2 shortcut above.
 */
PHP_METHOD(OceanMoon_Math_Rational, sqr)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_long num, den;
	rational_read_parts(Z_OBJ_P(ZEND_THIS), &num, &den);

	if (rational_calc_mul(num, den, num, den, return_value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */
