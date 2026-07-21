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
#include "rational_arginfo.h"
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
 * gcd() of two values already known to be genuine Rational numerator/denominator components --
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

/* {{{ rational_operand_components
 *
 * Shared by add()/sub()/mul()/div(): resolves $other (declared `Rational|int` in the stub) into a
 * (c, d) pair, alongside this object's own (a, b) -- matching the PHP package's
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

	zend_argument_type_error(1, "must be of type Rational|int, %s given", zend_zval_type_name(other));
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

	if (num == 0) {
		zend_throw_exception(math_ce_ArithmeticException, "Cannot divide by zero.", 0);
		RETURN_THROWS();
	}

	zend_result result = num > 0
		? rational_create(return_value, den, num)
		: rational_create(return_value, -den, -num);
	if (result == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::add(Rational|int $other): Rational
 *
 * Matches the PHP package's Rational::add(): (a/b) + (c/d) = (ad + bc) / (bd).
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

	zend_long ad, bc, h, k;
	if (rational_mul_checked(a, d, &ad) == FAILURE
		|| rational_mul_checked(b, c, &bc) == FAILURE
		|| rational_add_checked(ad, bc, &h) == FAILURE
		|| rational_mul_checked(b, d, &k) == FAILURE) {
		RETURN_THROWS();
	}

	if (rational_create(return_value, h, k) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::sub(Rational|int $other): Rational
 *
 * Matches the PHP package's Rational::sub(): (a/b) - (c/d) = (ad - bc) / (bd).
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

	zend_long ad, bc, h, k;
	if (rational_mul_checked(a, d, &ad) == FAILURE
		|| rational_mul_checked(b, c, &bc) == FAILURE
		|| rational_sub_checked(ad, bc, &h) == FAILURE
		|| rational_mul_checked(b, d, &k) == FAILURE) {
		RETURN_THROWS();
	}

	if (rational_create(return_value, h, k) == FAILURE) {
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
		RETURN_THROWS();
	}

	if (rational_create(return_value, h, k) == FAILURE) {
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

	/* Check for divide by 0. */
	if (Z_TYPE_P(other) == IS_LONG && Z_LVAL_P(other) == 0) {
		zend_throw_exception(math_ce_ArithmeticException, "Cannot divide by zero.", 0);
		RETURN_THROWS();
	}
	if (Z_TYPE_P(other) == IS_OBJECT && instanceof_function(Z_OBJCE_P(other), rational_ce_Rational)) {
		zend_long other_num, other_den;
		rational_read_parts(Z_OBJ_P(other), &other_num, &other_den);
		if (other_num == 0) {
			zend_throw_exception(math_ce_ArithmeticException, "Cannot divide by zero.", 0);
			RETURN_THROWS();
		}
	}

	/* Check for 0 divide by something. */
	zend_long self_num, self_den;
	rational_read_parts(Z_OBJ_P(ZEND_THIS), &self_num, &self_den);
	if (self_num == 0) {
		if (rational_create(return_value, 0, 1) == FAILURE) {
			RETURN_THROWS();
		}
		return;
	}

	zend_long a, b, c, d;
	if (rational_operand_components(Z_OBJ_P(ZEND_THIS), other, &a, &b, &c, &d) == FAILURE) {
		RETURN_THROWS();
	}

	zend_long gcd1 = rational_gcd2(a, c);
	if (gcd1 != 1) {
		a /= gcd1;
		c /= gcd1;
	}
	zend_long gcd2 = rational_gcd2(b, d);
	if (gcd2 != 1) {
		b /= gcd2;
		d /= gcd2;
	}

	zend_long h, k;
	if (rational_mul_checked(a, d, &h) == FAILURE || rational_mul_checked(b, c, &k) == FAILURE) {
		RETURN_THROWS();
	}

	if (rational_create(return_value, h, k) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */
