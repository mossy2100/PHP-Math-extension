/*
 * rational_power.c
 *
 * Power methods for OceanMoon\Math\Rational: pow(), sqr(). Mirrors the "Power methods" region of
 * the PHP package's Rational class, plus the exact-root helpers from its "Helper methods" region.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_smart_str.h"
#include "rational_internal.h"
#include "../../oceanmoon_math_arginfo.h"
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

/* The number of value bits in a zend_long (63 on 64-bit builds), and the largest root a binary
 * search in rational_exact_root() can need: the integer square root of ZEND_LONG_MAX. The PHP
 * package hard-codes the 64-bit values. */
#define RATIONAL_LONG_BITS (SIZEOF_ZEND_LONG * 8 - 1)
#if SIZEOF_ZEND_LONG == 8
# define RATIONAL_MAX_ROOT 3037000499
#else
# define RATIONAL_MAX_ROOT 46340
#endif

/* {{{ rational_ratio_to_str
 *
 * Formats num/den like __toString(): just the numerator if den is 1. Returns a new zend_string
 * the caller owns. Shared with the float ** forms (rational_operators.c).
 */
zend_string *rational_ratio_to_str(zend_long num, zend_long den)
{
	smart_str buf = {0};
	smart_str_append_long(&buf, num);
	if (den != 1) {
		smart_str_appendc(&buf, '/');
		smart_str_append_long(&buf, den);
	}
	smart_str_0(&buf);
	return buf.s;
}
/* }}} */

/* {{{ rational_throw_pow_error
 *
 * Throws an exception for a power with no representable result, matching the PHP package's
 * message: "Cannot raise <base> to power <exp>. <reason>". Takes ownership of (and releases) base
 * and exp. Shared with the float ** forms (rational_operators.c).
 */
void rational_throw_pow_error(zend_class_entry *ce, zend_string *base, zend_string *exp, const char *reason)
{
	zend_throw_exception_ex(ce, 0, "Cannot raise %s to power %s. %s", ZSTR_VAL(base), ZSTR_VAL(exp), reason);
	zend_string_release(base);
	zend_string_release(exp);
}
/* }}} */

/* {{{ rational_compare_power
 *
 * Compares k^q with n without overflowing, matching the PHP package's comparePower(). All three
 * arguments must be positive. Returns -1, 0, or 1.
 */
static int rational_compare_power(zend_long k, zend_long q, zend_long n)
{
	zend_long power = 1;
	for (zend_long i = 0; i < q; i++) {
		/* If multiplying by k again would exceed n, k^q > n. Checking against n / k first means
		 * the multiplication below never overflows. */
		if (power > n / k) {
			return 1;
		}
		power *= k;
	}
	return power < n ? -1 : (power > n ? 1 : 0);
}
/* }}} */

/* {{{ rational_exact_root
 *
 * Finds the exact integer q-th root of n, if there is one, matching the PHP package's
 * exactRoot(). q must be positive. Sets *out and returns true if there's an integer k with
 * k^q == n; returns false if not (including an even root of a negative number, which isn't
 * real). Uses integer arithmetic only.
 *
 * Unlike the package, this handles n == ZEND_LONG_MIN: a Rational's numerator can never be
 * ZEND_LONG_MIN, but the base of `int ** Rational` can.
 */
static bool rational_exact_root(zend_long n, zend_long q, zend_long *out)
{
	/* Every integer is its own first root, and 0 and 1 are their own roots of any degree. */
	if (q == 1 || n == 0 || n == 1) {
		*out = n;
		return true;
	}

	/* Handle a negative number. */
	if (n < 0) {
		/* An even root of a negative number isn't real. */
		if (q % 2 == 0) {
			return false;
		}

		/* |ZEND_LONG_MIN| is 2^63, which doesn't fit in a zend_long, so -n can't be calculated.
		 * Handle it directly instead: 2^63 has an exact q-th root only if q divides 63, in which
		 * case the root is 2^(63/q). q is odd and at least 3 here, so the shift is at most 21. */
		if (n == ZEND_LONG_MIN) {
			if (RATIONAL_LONG_BITS % q != 0) {
				return false;
			}
			*out = -((zend_long) 1 << (RATIONAL_LONG_BITS / q));
			return true;
		}

		/* An odd root of a negative number is the negative of the root of its magnitude. */
		zend_long root;
		if (!rational_exact_root(-n, q, &root)) {
			return false;
		}
		*out = -root;
		return true;
	}

	/* From here, n >= 2. Since 2^63 > ZEND_LONG_MAX, no zend_long >= 2 has an exact root of
	 * degree 63 or more. */
	if (q >= RATIONAL_LONG_BITS) {
		return false;
	}

	/* Binary search for the root. Since q >= 2, the root can't exceed the square root of
	 * ZEND_LONG_MAX. */
	zend_long low = 1;
	zend_long high = n < RATIONAL_MAX_ROOT ? n : RATIONAL_MAX_ROOT;
	while (low <= high) {
		zend_long mid = low + (high - low) / 2;
		int cmp = rational_compare_power(mid, q, n);
		if (cmp == 0) {
			*out = mid;
			return true;
		}
		if (cmp < 0) {
			low = mid + 1;
		} else {
			high = mid - 1;
		}
	}

	return false;
}
/* }}} */

/* {{{ rational_calc_pow
 *
 * The computational core of pow(), factored out so it can recurse on itself (matching the PHP
 * source's own `$this->pow(...)` recursive calls) without going through a full PHP method
 * dispatch each time.
 *
 * Matches the PHP package's Rational::pow() exactly, including its shortcuts (exponent 0, base 0,
 * exponent 1/2/-1, exponent ZEND_LONG_MIN) - the exponent-2 and exponent-ZEND_LONG_MIN branches
 * reuse rational_calc_mul()/rational_calc_inv() (rational_arithmetic.c) directly, same as the PHP
 * source's sqr()/mul()/inv() delegations.
 */
zend_result rational_calc_pow(zend_long num, zend_long den, zend_long exponent, zval *return_value)
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

	/* Exponent 1: any number to power 1 is itself - a new, distinct instance (matching the PHP
	 * source's `clone $this`), not literally the same object. */
	if (exponent == 1) {
		return rational_create(return_value, num, den);
	}

	/* Exponent 2: delegate to sqr() (mul(this, this) - reuses rational_calc_mul() directly). */
	if (exponent == 2) {
		return rational_calc_mul(num, den, num, den, return_value);
	}

	/* Exponent -1: delegate to inv(). Safe - base != 0 already established above. */
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

/* {{{ rational_calc_pow_rational
 *
 * pow() with a Rational exponent p/q (in lowest terms), matching the PHP package: the q-th root,
 * raised to the power p. The result is rational only if the base's numerator and denominator
 * both have exact q-th roots; otherwise this throws a DomainException. Shared with the
 * `Rational ** Rational` operator (rational_operators.c).
 */
zend_result rational_calc_pow_rational(zend_long num, zend_long den, zend_long p, zend_long q, zval *return_value)
{
	/* A whole-number exponent is handled the same as an int. */
	if (q == 1) {
		return rational_calc_pow(num, den, p, return_value);
	}

	zend_long root_num, root_den;
	if (!rational_exact_root(num, q, &root_num) || !rational_exact_root(den, q, &root_den)) {
		rational_throw_pow_error(spl_ce_DomainException, rational_ratio_to_str(num, den), rational_ratio_to_str(p, q),
			num < 0 && q % 2 == 0 ? RATIONAL_POW_NOT_REAL : RATIONAL_POW_IRRATIONAL);
		return FAILURE;
	}

	/* The roots of a reduced fraction's parts are themselves coprime, and the root of the
	 * positive denominator is positive, so root_num/root_den is already in canonical form. */
	return rational_calc_pow(root_num, root_den, p, return_value);
}
/* }}} */

/* {{{ rational_calc_int_pow_rational
 *
 * The `int ** Rational` operator (rational_operators.c), with the Rational exponent p/q in lowest
 * terms. Same rules as rational_calc_pow_rational(): the result is a Rational, and only if the base
 * has an exact q-th root; otherwise this throws a DomainException.
 *
 * The base doesn't widen to a Rational first, because a Rational can't hold ZEND_LONG_MIN, though
 * ZEND_LONG_MIN does have exact roots (e.g. its cube root is -2^21). Only the root is converted,
 * through rational_create(), so a root that's itself ZEND_LONG_MIN (exponent 1/1) throws the
 * constructor's usual exception.
 */
zend_result rational_calc_int_pow_rational(zend_long base, zend_long p, zend_long q, zval *return_value)
{
	zend_long root;
	if (!rational_exact_root(base, q, &root)) {
		rational_throw_pow_error(spl_ce_DomainException, rational_ratio_to_str(base, 1), rational_ratio_to_str(p, q),
			base < 0 && q % 2 == 0 ? RATIONAL_POW_NOT_REAL : RATIONAL_POW_IRRATIONAL);
		return FAILURE;
	}

	zval root_zv;
	if (rational_create(&root_zv, root, 1) == FAILURE) {
		return FAILURE;
	}
	zend_long root_num, root_den;
	rational_read_parts(Z_OBJ(root_zv), &root_num, &root_den);
	zval_ptr_dtor(&root_zv);

	return rational_calc_pow(root_num, root_den, p, return_value);
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::pow(int|Rational $exp): Rational
 *
 * Matches the PHP package's Rational::pow(): an int or Rational exponent gives an exact Rational.
 * Like add()/sub()/mul()/div() (rational_arithmetic.c), the union parameter is checked by hand,
 * since an internal function's zval parameter isn't validated against its arginfo type.
 */
PHP_METHOD(OceanMoon_Math_Rational, pow)
{
	zval *exp;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(exp)
	ZEND_PARSE_PARAMETERS_END();

	zend_long num, den;
	rational_read_parts(Z_OBJ_P(ZEND_THIS), &num, &den);

	zend_result result;
	if (Z_TYPE_P(exp) == IS_LONG) {
		result = rational_calc_pow(num, den, Z_LVAL_P(exp), return_value);
	} else if (Z_TYPE_P(exp) == IS_OBJECT && instanceof_function(Z_OBJCE_P(exp), rational_ce_Rational)) {
		zend_long p, q;
		rational_read_parts(Z_OBJ_P(exp), &p, &q);
		result = rational_calc_pow_rational(num, den, p, q, return_value);
	} else {
		zend_argument_type_error(1, "must be of type Rational|int, %s given", zend_zval_type_name(exp));
		RETURN_THROWS();
	}

	if (result == FAILURE) {
		math_rethrow_overflow("raise the given Rational to the given power");
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::sqr(): Rational
 *
 * Matches the PHP package's Rational::sqr(): this * this. Equivalent to pow(2), but more
 * efficient/readable - reuses rational_calc_mul() directly, same as rational_calc_pow()'s own
 * exponent-2 shortcut above.
 */
PHP_METHOD(OceanMoon_Math_Rational, sqr)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_long num, den;
	rational_read_parts(Z_OBJ_P(ZEND_THIS), &num, &den);

	if (rational_calc_mul(num, den, num, den, return_value) == FAILURE) {
		math_rethrow_overflow("square the given Rational");
		RETURN_THROWS();
	}
}
/* }}} */
