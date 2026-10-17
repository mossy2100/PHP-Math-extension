/*
 * rational_arithmetic.c
 *
 * Unary and binary arithmetic methods for OceanMoon\Math\Rational: abs(), neg(), inv(), add(),
 * sub(), mul(), div(). Mirrors the "Unary arithmetic methods"/"Binary arithmetic methods" regions
 * of the PHP package's Rational class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "rational_internal.h"
#include "../../oceanmoon_math_arginfo.h"
#include "exceptions.h"
#include "integers.h"

/* {{{ rational_add_checked/rational_sub_checked/rational_mul_checked
 *
 * Thin wrappers around math_integers_add()/sub()/mul() that throw OverflowException with the
 * exact message Core\Integers::add()/sub()/mul() themselves use, matching the PHP package's
 * arithmetic methods (which call those directly and let the OverflowException propagate).
 */
static zend_result rational_add_checked(zend_long a, zend_long b, zend_long *out)
{
	if (math_integers_add(a, b, out) != MATH_INTEGERS_OK) {
		zend_throw_exception(spl_ce_OverflowException, "Overflow in integer addition.", 0);
		return FAILURE;
	}
	return SUCCESS;
}

static zend_result rational_sub_checked(zend_long a, zend_long b, zend_long *out)
{
	if (math_integers_sub(a, b, out) != MATH_INTEGERS_OK) {
		zend_throw_exception(spl_ce_OverflowException, "Overflow in integer subtraction.", 0);
		return FAILURE;
	}
	return SUCCESS;
}

static zend_result rational_mul_checked(zend_long a, zend_long b, zend_long *out)
{
	if (math_integers_mul(a, b, out) != MATH_INTEGERS_OK) {
		zend_throw_exception(spl_ce_OverflowException, "Overflow in integer multiplication.", 0);
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

/* {{{ rational_gcd2
 *
 * gcd() of two values already known to be genuine Rational numerator/denominator components -
 * by this class's own invariant, never both 0 or ZEND_LONG_MIN, so math_integers_gcd() can't
 * actually fail for these call sites (unlike rational_init(), which must handle raw, possibly
 * invalid caller-supplied longs). Falls back to 1 (no reduction) if that reasoning is ever wrong,
 * rather than trusting it silently.
 */
static zend_long rational_gcd2(zend_long a, zend_long b)
{
	zend_long nums[2] = {a, b};
	zend_long gcd;
	if (math_integers_gcd(nums, 2, &gcd) != MATH_INTEGERS_OK) {
		return 1;
	}
	return gcd;
}
/* }}} */

/* {{{ rational_calc_mul
 *
 * The computational core of mul(), factored out (declared in rational_internal.h) so div() below
 * and pow() (rational_power.c) can reuse the same gcd-based cross-cancellation instead of
 * duplicating it: (a/b) * (c/d) = ac/bd, cancelling gcd(a,d) and gcd(b,c) first to avert overflow.
 * div() calls this as rational_calc_mul(a, b, d, c, ...) - (a/b) / (c/d) = (a/b) * (d/c) - and
 * pow()'s exponent-2/exponent-PHP_INT_MIN shortcuts call it directly too.
 */
zend_result rational_calc_mul(zend_long a, zend_long b, zend_long c, zend_long d, zval *return_value)
{
	zend_long gcd1 = rational_gcd2(a, d);
	if (gcd1 != 1) {
		a /= gcd1;
		d /= gcd1;
	}
	zend_long gcd2 = rational_gcd2(b, c);
	if (gcd2 != 1) {
		b /= gcd2;
		c /= gcd2;
	}

	zend_long h, k;
	if (rational_mul_checked(a, c, &h) == FAILURE || rational_mul_checked(b, d, &k) == FAILURE) {
		return FAILURE;
	}

	return rational_create(return_value, h, k);
}
/* }}} */

/* {{{ rational_calc_add_sub
 *
 * The computational core of add()/sub(), declared in rational_internal.h so the + and - operators
 * (rational_operators.c) can reuse it. Matches the PHP package's Rational::calcAddSub(): to keep the
 * intermediate values small, the integer parts are added separately, and the fractions are added
 * with Knuth's method, which divides out the gcd of the denominators before multiplying. b and d must
 * be positive.
 */
zend_result rational_calc_add_sub(zend_long a, zend_long b, zend_long c, zend_long d, bool subtract, zval *return_value)
{
	/* Split each value into an integer part and a fraction, e.g. 7/2 = 3 + 1/2, and add them separately. */
	zend_long int_part;
	if ((subtract ? rational_sub_checked(a / b, c / d, &int_part) : rational_add_checked(a / b, c / d, &int_part))
		== FAILURE) {
		return FAILURE;
	}
	a %= b;
	c %= d;

	/* Let g = gcd(b, d). Then (a/b) +- (c/d) = t / ((b/g)*d), where t = a*(d/g) +- c*(b/g). */
	zend_long g = rational_gcd2(b, d);
	zend_long b_over_g = b / g;
	zend_long ad, cb, t;
	if (rational_mul_checked(a, d / g, &ad) == FAILURE || rational_mul_checked(c, b_over_g, &cb) == FAILURE) {
		return FAILURE;
	}
	if ((subtract ? rational_sub_checked(ad, cb, &t) : rational_add_checked(ad, cb, &t)) == FAILURE) {
		return FAILURE;
	}

	/* t has no common factor with b/g or d/g, so the only factor to cancel is gcd(t, g). The result is reduced. */
	zend_long g2 = rational_gcd2(t, g);
	zend_long num = t / g2;
	zend_long den, scaled, h;
	if (rational_mul_checked(b_over_g, d / g2, &den) == FAILURE) {
		return FAILURE;
	}

	/* Add the integer part. */
	if (rational_mul_checked(int_part, den, &scaled) == FAILURE || rational_add_checked(scaled, num, &h) == FAILURE) {
		return FAILURE;
	}

	return rational_create(return_value, h, den);
}
/* }}} */

/* {{{ rational_calc_inv
 *
 * The computational core of inv(), factored out (declared in rational_internal.h) so pow()'s
 * exponent-(-1)/exponent-PHP_INT_MIN shortcuts (rational_power.c) can reuse it directly.
 */
zend_result rational_calc_inv(zend_long num, zend_long den, zval *return_value)
{
	if (num == 0) {
		zend_throw_exception(math_ce_ArithmeticException, "Cannot divide by zero.", 0);
		return FAILURE;
	}

	return num > 0
		? rational_create(return_value, den, num)
		: rational_create(return_value, -den, -num);
}
/* }}} */

/* {{{ rational_operand_components
 *
 * Shared by add()/sub()/mul()/div(): resolves $other (declared `Rational|int` in the stub) into a
 * (c, d) pair, alongside this object's own (a, b) - matching the PHP package's
 * getOperandComponents(). Unlike a userland-typed parameter, an internal function's zval
 * parameter isn't automatically validated against its declared arginfo type before the function
 * body runs, so this does it manually, matching the pattern used throughout the Complex/ files.
 */
static zend_result rational_operand_components(
	zend_object *self_obj, zval *other, zend_long *a, zend_long *b, zend_long *c, zend_long *d)
{
	rational_read_parts(self_obj, a, b);

	if (Z_TYPE_P(other) == IS_LONG) {
		*c = Z_LVAL_P(other);
		*d = 1;
		return SUCCESS;
	}

	if (Z_TYPE_P(other) == IS_OBJECT && instanceof_function(Z_OBJCE_P(other), rational_ce_Rational)) {
		rational_read_parts(Z_OBJ_P(other), c, d);
		return SUCCESS;
	}

	zend_argument_type_error(1, "must be of type OceanMoon\\Math\\Rational|int, %s given", zend_zval_value_name(other));
	return FAILURE;
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::abs(): Rational
 *
 * Matches the PHP package's Rational::abs(). Negating numerator is always safe: this class's own
 * invariant guarantees numerator is never ZEND_LONG_MIN.
 */
PHP_METHOD(OceanMoon_Math_Rational, abs)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_long num, den;
	rational_read_parts(Z_OBJ_P(ZEND_THIS), &num, &den);

	if (rational_create(return_value, num < 0 ? -num : num, den) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::neg(): Rational
 *
 * Matches the PHP package's Rational::neg().
 */
PHP_METHOD(OceanMoon_Math_Rational, neg)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_long num, den;
	rational_read_parts(Z_OBJ_P(ZEND_THIS), &num, &den);

	if (rational_create(return_value, -num, den) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::inv(): Rational
 *
 * Matches the PHP package's Rational::inv(): the reciprocal, preserving sign (numerator/
 * denominator swap, negating both if the numerator was negative so the denominator stays
 * positive).
 */
PHP_METHOD(OceanMoon_Math_Rational, inv)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_long num, den;
	rational_read_parts(Z_OBJ_P(ZEND_THIS), &num, &den);

	if (rational_calc_inv(num, den, return_value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::add(Rational|int $other): Rational
 *
 * Matches the PHP package's Rational::add(), via rational_calc_add_sub().
 */
PHP_METHOD(OceanMoon_Math_Rational, add)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(other)
	ZEND_PARSE_PARAMETERS_END();

	zend_long a, b, c, d;
	if (rational_operand_components(Z_OBJ_P(ZEND_THIS), other, &a, &b, &c, &d) == FAILURE) {
		RETURN_THROWS();
	}

	if (rational_calc_add_sub(a, b, c, d, false, return_value) == FAILURE) {
		math_rethrow_overflow("add the given Rational numbers");
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::sub(Rational|int $other): Rational
 *
 * Matches the PHP package's Rational::sub(), via rational_calc_add_sub().
 */
PHP_METHOD(OceanMoon_Math_Rational, sub)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(other)
	ZEND_PARSE_PARAMETERS_END();

	zend_long a, b, c, d;
	if (rational_operand_components(Z_OBJ_P(ZEND_THIS), other, &a, &b, &c, &d) == FAILURE) {
		RETURN_THROWS();
	}

	if (rational_calc_add_sub(a, b, c, d, true, return_value) == FAILURE) {
		math_rethrow_overflow("subtract the given Rational numbers");
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::mul(Rational|int $other): Rational
 *
 * Matches the PHP package's Rational::mul(): (a/b) * (c/d) = ac/bd, cross-cancelling gcd(a,d) and
 * gcd(b,c) first to avert overflow.
 */
PHP_METHOD(OceanMoon_Math_Rational, mul)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(other)
	ZEND_PARSE_PARAMETERS_END();

	zend_long a, b, c, d;
	if (rational_operand_components(Z_OBJ_P(ZEND_THIS), other, &a, &b, &c, &d) == FAILURE) {
		RETURN_THROWS();
	}

	if (rational_calc_mul(a, b, c, d, return_value) == FAILURE) {
		math_rethrow_overflow("multiply the given Rational numbers");
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::div(Rational|int $other): Rational
 *
 * Matches the PHP package's Rational::div(): (a/b) / (c/d) = (a/b) * (d/c) = ad/bc, cross-
 * cancelling gcd(a,c) and gcd(b,d) first to avert overflow.
 */
PHP_METHOD(OceanMoon_Math_Rational, div)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(other)
	ZEND_PARSE_PARAMETERS_END();

	/* Check the type first, as the package's `self|int` parameter type does. */
	zend_long a, b, c, d;
	if (rational_operand_components(Z_OBJ_P(ZEND_THIS), other, &a, &b, &c, &d) == FAILURE) {
		RETURN_THROWS();
	}

	/* Check for divide by 0. */
	if (c == 0) {
		zend_throw_exception(math_ce_ArithmeticException, "Cannot divide by zero.", 0);
		RETURN_THROWS();
	}

	/* Check for 0 divide by something. */
	if (a == 0) {
		if (rational_create(return_value, 0, 1) == FAILURE) {
			RETURN_THROWS();
		}
		return;
	}

	/* (a/b) / (c/d) = (a/b) * (d/c) - reuse rational_calc_mul() with c/d swapped. */
	if (rational_calc_mul(a, b, d, c, return_value) == FAILURE) {
		math_rethrow_overflow("divide the given Rational numbers");
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ rational_calc_mod
 *
 * The computational core of mod() for an int or Rational divisor c/d, which isn't zero. Shared with the % operator
 * (rational_operators.c). Throws OverflowException with the generic integer message, for the caller to replace via
 * math_rethrow_overflow(), if an intermediate value overflows.
 */
zend_result rational_calc_mod(zend_long a, zend_long b, zend_long c, zend_long d, zval *return_value)
{
	/* Over the common denominator lcm(b, d), this Rational is A/lcm and the divisor is C/lcm, so the remainder is
	 * (A % C)/lcm. */
	zend_long gcd = rational_gcd2(b, d);
	zend_long b_over_gcd = b / gcd;
	zend_long big_a, big_c, lcm;
	if (rational_mul_checked(a, d / gcd, &big_a) == FAILURE
		|| rational_mul_checked(c, b_over_gcd, &big_c) == FAILURE
		|| rational_mul_checked(b_over_gcd, d, &lcm) == FAILURE) {
		return FAILURE;
	}

	/* C's % truncates, giving the remainder the sign of the dividend, as PHP's % does. ZEND_LONG_MIN % -1 overflows
	 * in C, though the remainder is 0, so a divisor of -1 is handled first. */
	return rational_create(return_value, big_c == -1 ? 0 : big_a % big_c, lcm);
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::mod(Rational|int $other): Rational
 *
 * Matches the PHP package's Rational::mod(). The result has the sign of the dividend, as with PHP's % and fmod().
 */
PHP_METHOD(OceanMoon_Math_Rational, mod)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(other)
	ZEND_PARSE_PARAMETERS_END();

	zend_long a, b, c, d;
	if (rational_operand_components(Z_OBJ_P(ZEND_THIS), other, &a, &b, &c, &d) == FAILURE) {
		RETURN_THROWS();
	}

	/* Check for divide by 0. */
	if (c == 0) {
		zend_throw_exception(math_ce_ArithmeticException, "Cannot divide by zero.", 0);
		RETURN_THROWS();
	}

	if (rational_calc_mod(a, b, c, d, return_value) == FAILURE) {
		math_rethrow_overflow("find the remainder of the given Rational numbers");
		RETURN_THROWS();
	}
}
/* }}} */
