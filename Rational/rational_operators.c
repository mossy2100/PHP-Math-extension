/*
 * rational_operators.c
 *
 * Operator overloading for OceanMoon\Math\Rational: +, -, *, /, ** (each accepting a Rational,
 * int, or float operand -- see below for how a float operand changes the result type), plus
 * unary - (negate), and the comparison operators <, <=, >, >=, <=>, ==, != (all backed by a
 * single `compare` object handler). Mirrors docs/Rational.md in this repo -- these operators
 * only exist here in the extension; the userland package has no way to overload operators, so
 * it exposes the same behavior only as named methods (add()/sub()/mul()/div()/pow()/neg()/
 * compare()/etc.), none of which accept a float operand (see docs/Rational.md's "Numeric type
 * widening" section for why the operators can support float where the methods can't).
 *
 * Numeric widening: when either operand of +, -, *, /, or ** is a genuine float, both operands
 * widen to float and the result is an ordinary float -- matching how int widens to float
 * whenever a float is involved in plain PHP arithmetic. When no float is involved, an int
 * operand instead widens to Rational, and the result stays an exact Rational.
 *
 * Also owns installing the custom object handlers table (do_operation/compare) on
 * Rational's class entry -- unlike Complex, Rational has no lazy computed properties needing
 * their own read_property override, so there's no separate rational_properties.c; this file's
 * rational_operators_minit() is the only place Rational needs a non-default handlers table at
 * all.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <math.h>
#include <string.h>

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "rational_internal.h"
#include "rational_arginfo.h"
#include "exceptions.h"
#include "integers.h"

/* {{{ rop_add_checked/rop_sub_checked/rop_mul_checked
 *
 * Overflow-checked add/sub/mul for the ADD/SUB operator cases below, throwing with the same
 * messages Core\Integers::add()/sub()/mul() use -- duplicated from the near-identical helpers in
 * rational_arithmetic.c rather than exposing those too, matching how Complex/complex_operators.c
 * reimplements its own simple arithmetic inline instead of calling into complex_arithmetic.c: the
 * MUL case here doesn't need this (it goes through rational_calc_mul() directly, which already
 * has its own equivalent internally), only ADD/SUB's `ad`/`bc`/`h`/`k` formula does.
 */
static zend_result rop_add_checked(zend_long a, zend_long b, zend_long *out)
{
	if (math_integers_add(a, b, out) != MATH_INTEGERS_OK) {
		zend_throw_exception(spl_ce_OverflowException, "Overflow in integer addition.", 0);
		return FAILURE;
	}
	return SUCCESS;
}

static zend_result rop_sub_checked(zend_long a, zend_long b, zend_long *out)
{
	if (math_integers_sub(a, b, out) != MATH_INTEGERS_OK) {
		zend_throw_exception(spl_ce_OverflowException, "Overflow in integer subtraction.", 0);
		return FAILURE;
	}
	return SUCCESS;
}

static zend_result rop_mul_checked(zend_long a, zend_long b, zend_long *out)
{
	if (math_integers_mul(a, b, out) != MATH_INTEGERS_OK) {
		zend_throw_exception(spl_ce_OverflowException, "Overflow in integer multiplication.", 0);
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

/* {{{ rational_operand_to_parts
 *
 * Resolves an operator operand (Rational or int -- anything else, including float, is
 * unsupported here) into a (numerator, denominator) pair. Used for the exact-Rational path of
 * +, -, *, / and the `Rational ** int` case of ** -- once rational_do_operation() has already
 * established neither operand is a float (see its doc comment), an int operand always widens to
 * Rational exactly, matching add()/sub()/mul()/div()'s own `Rational|int` signatures.
 *
 * Never throws a TypeError for a wrong type -- returns FAILURE without an exception, so the
 * caller can return FAILURE from the do_operation handler in turn, letting the engine fall
 * through to its own standard "Unsupported operand types" TypeError.
 */
static zend_result rational_operand_to_parts(zval *zv, zend_long *out_num, zend_long *out_den)
{
	if (Z_TYPE_P(zv) == IS_OBJECT && instanceof_function(Z_OBJCE_P(zv), rational_ce_Rational)) {
		rational_read_parts(Z_OBJ_P(zv), out_num, out_den);
		return SUCCESS;
	}

	if (Z_TYPE_P(zv) == IS_LONG) {
		*out_num = Z_LVAL_P(zv);
		*out_den = 1;
		return SUCCESS;
	}

	return FAILURE;
}
/* }}} */

/* {{{ rational_operand_to_double
 *
 * Resolves an operator operand (Rational, int, or float -- anything else is unsupported) to its
 * float value, for the float-widening path of +, -, *, /, and **. A Rational widens via ordinary
 * numerator/denominator division (matching toFloat(), rational_conversion.c), not through
 * rational_calc_from_float() -- that function does the reverse conversion (float -> Rational)
 * and doesn't apply here.
 *
 * Never throws a TypeError for a wrong type -- returns FAILURE without an exception, matching
 * rational_operand_to_parts() above.
 */
static zend_result rational_operand_to_double(zval *zv, double *out)
{
	if (Z_TYPE_P(zv) == IS_DOUBLE) {
		*out = Z_DVAL_P(zv);
		return SUCCESS;
	}

	if (Z_TYPE_P(zv) == IS_LONG) {
		*out = (double) Z_LVAL_P(zv);
		return SUCCESS;
	}

	if (Z_TYPE_P(zv) == IS_OBJECT && instanceof_function(Z_OBJCE_P(zv), rational_ce_Rational)) {
		zend_long num, den;
		rational_read_parts(Z_OBJ_P(zv), &num, &den);
		*out = (double) num / (double) den;
		return SUCCESS;
	}

	return FAILURE;
}
/* }}} */

/* {{{ rational_do_float_operation
 *
 * The float-widened path of +, -, *, / for when either operand is a genuine float (see
 * rational_do_operation()'s doc comment). Division by zero -- including float 0.0 -- still
 * throws ArithmeticException here rather than producing INF/-INF/NAN, deliberately departing
 * from ordinary float division (see docs/Rational.md's "/ (divide)" section).
 */
static zend_result rational_do_float_operation(uint8_t opcode, zval *result, zval *op1, zval *op2)
{
	double a, b;
	if (rational_operand_to_double(op1, &a) == FAILURE) {
		return FAILURE;
	}
	if (rational_operand_to_double(op2, &b) == FAILURE) {
		return FAILURE;
	}

	switch (opcode) {
		case ZEND_ADD:
			ZVAL_DOUBLE(result, a + b);
			return SUCCESS;
		case ZEND_SUB:
			ZVAL_DOUBLE(result, a - b);
			return SUCCESS;
		case ZEND_MUL:
			ZVAL_DOUBLE(result, a * b);
			return SUCCESS;
		case ZEND_DIV:
			if (b == 0.0) {
				zend_throw_exception(math_ce_ArithmeticException, "Cannot divide by zero.", 0);
				return FAILURE;
			}
			ZVAL_DOUBLE(result, a / b);
			return SUCCESS;
		default:
			return FAILURE;
	}
}
/* }}} */

/* {{{ rational_do_pow
 *
 * The ** case of rational_do_operation(), split out for readability. Three forms are supported:
 *
 *   1. `Rational ** int`   -- exact aⁿ/bⁿ algorithm, via rational_calc_pow().
 *   2. `Rational ** float` -- both operands widen to float, per docs/Rational.md.
 *   3. `float ** Rational` -- likewise widens to float.
 *
 * `Rational ** Rational` and `int ** Rational` are both unsupported (FAILURE): int only ever
 * widens to Rational, never straight to float, and a Rational exponent has no exact Rational
 * result -- so an int base has no path forward once the exponent is Rational (see
 * docs/Rational.md's "** (power)" section for the fuller reasoning).
 */
static zend_result rational_do_pow(zval *result, zval *op1, zval *op2)
{
	bool op1_is_rational = Z_TYPE_P(op1) == IS_OBJECT && instanceof_function(Z_OBJCE_P(op1), rational_ce_Rational);
	bool op2_is_rational = Z_TYPE_P(op2) == IS_OBJECT && instanceof_function(Z_OBJCE_P(op2), rational_ce_Rational);

	if (op1_is_rational && Z_TYPE_P(op2) == IS_LONG) {
		zend_long num, den;
		rational_read_parts(Z_OBJ_P(op1), &num, &den);
		return rational_calc_pow(num, den, Z_LVAL_P(op2), result);
	}

	if ((op1_is_rational && Z_TYPE_P(op2) == IS_DOUBLE) || (Z_TYPE_P(op1) == IS_DOUBLE && op2_is_rational)) {
		double base, exponent;
		if (rational_operand_to_double(op1, &base) == FAILURE) {
			return FAILURE;
		}
		if (rational_operand_to_double(op2, &exponent) == FAILURE) {
			return FAILURE;
		}
		ZVAL_DOUBLE(result, pow(base, exponent));
		return SUCCESS;
	}

	return FAILURE;
}
/* }}} */

/* {{{ rational_do_operation
 *
 * The `do_operation` object handler backing Rational's arithmetic operator overloads: +, -, *, /
 * for Rational|int|float on either side, and ** (see rational_do_pow()'s doc comment for its
 * three supported forms). Unary - needs no special-casing here: PHP has no dedicated opcode for
 * it, and the compiler lowers `-$r` to `$r * -1` (see zend_compile_unary_pm() in
 * Zend/zend_compile.c and complex_do_operation()'s doc comment in Complex/complex_operators.c
 * for the fuller explanation), so it's already covered by the ZEND_MUL case. (Unary + similarly
 * lowers to `$r * 1`.)
 *
 * For +, -, *, /: if either operand is a genuine float, both widen to float and
 * rational_do_float_operation() handles it (see docs/Rational.md's "Numeric type widening"
 * section); otherwise an int operand widens to Rational and the exact-Rational path below
 * applies, matching add()/sub()/mul()/div().
 */
zend_result rational_do_operation(uint8_t opcode, zval *result, zval *op1, zval *op2)
{
	if (opcode == ZEND_POW) {
		return rational_do_pow(result, op1, op2);
	}

	if (opcode != ZEND_ADD && opcode != ZEND_SUB && opcode != ZEND_MUL && opcode != ZEND_DIV) {
		return FAILURE;
	}

	if (Z_TYPE_P(op1) == IS_DOUBLE || Z_TYPE_P(op2) == IS_DOUBLE) {
		return rational_do_float_operation(opcode, result, op1, op2);
	}

	zend_long a, b, c, d;
	if (rational_operand_to_parts(op1, &a, &b) == FAILURE) {
		return FAILURE;
	}
	if (rational_operand_to_parts(op2, &c, &d) == FAILURE) {
		return FAILURE;
	}

	switch (opcode) {
		case ZEND_ADD: {
			zend_long ad, bc, h, k;
			if (rop_mul_checked(a, d, &ad) == FAILURE
				|| rop_mul_checked(b, c, &bc) == FAILURE
				|| rop_add_checked(ad, bc, &h) == FAILURE
				|| rop_mul_checked(b, d, &k) == FAILURE) {
				return FAILURE;
			}
			return rational_create(result, h, k);
		}
		case ZEND_SUB: {
			zend_long ad, bc, h, k;
			if (rop_mul_checked(a, d, &ad) == FAILURE
				|| rop_mul_checked(b, c, &bc) == FAILURE
				|| rop_sub_checked(ad, bc, &h) == FAILURE
				|| rop_mul_checked(b, d, &k) == FAILURE) {
				return FAILURE;
			}
			return rational_create(result, h, k);
		}
		case ZEND_MUL:
			return rational_calc_mul(a, b, c, d, result);
		case ZEND_DIV: {
			if (c == 0) {
				zend_throw_exception(math_ce_ArithmeticException, "Cannot divide by zero.", 0);
				return FAILURE;
			}
			/* (a/b) / (c/d) = (a/b) * (d/c) -- reuse rational_calc_mul() with c/d swapped. */
			return rational_calc_mul(a, b, d, c, result);
		}
		default:
			return FAILURE;
	}
}
/* }}} */

/* {{{ rational_do_compare
 *
 * The `compare` object handler backing Rational's comparison operators: <, <=, >, >=, <=>, and
 * (as an unavoidable side effect of PHP wiring all of these to the same handler) ==/!=. Never
 * affects ===/!==, which always mean reference identity for objects regardless of this handler
 * (see zend_is_identical() in Zend/zend_operators.c).
 *
 * Called with the original op1/op2 in their expression order, whichever side the Rational
 * instance is actually on (PHP picks op1's handler if op1 is a suitable object, else op2's -- see
 * zend_compare()'s IS_OBJECT branch in Zend/zend_operators.c) -- so this determines which side is
 * `self` and, if it's op2 (e.g. `5 <=> $r`), negates the result to correct for comparing in the
 * opposite direction from what rational_calc_compare() (self vs. other) computes.
 *
 * Delegates entirely to rational_calc_compare() (rational_comparison.c) -- the same core
 * compare()/equal()/etc. use -- so operator and method comparisons are guaranteed identical,
 * including exception behavior (InvalidArgumentException for a bad type, DomainException for
 * NAN). If rational_calc_compare() fails, an exception is already pending; the returned int value
 * is irrelevant once that's true (the engine checks for a pending exception after calling this
 * handler and aborts before using the result).
 */
int rational_do_compare(zval *op1, zval *op2)
{
	zval *self_zv, *other;
	bool self_is_op1;

	if (Z_TYPE_P(op1) == IS_OBJECT && instanceof_function(Z_OBJCE_P(op1), rational_ce_Rational)) {
		self_zv = op1;
		other = op2;
		self_is_op1 = true;
	} else {
		self_zv = op2;
		other = op1;
		self_is_op1 = false;
	}

	int cmp;
	if (rational_calc_compare(Z_OBJ_P(self_zv), other, &cmp) == FAILURE) {
		return 0;
	}

	return self_is_op1 ? cmp : -cmp;
}
/* }}} */

/* The custom object handlers for Rational, installed by rational_create_object(). A copy of the
 * standard handlers with `do_operation`/`compare` overridden, matching how
 * Complex/complex_properties.c installs Complex's (which also overrides `read_property`, for its
 * lazy computed properties -- Rational has none, so this is the only override it needs). */
static zend_object_handlers rational_object_handlers;

/* {{{ rational_create_object
 *
 * Installs rational_object_handlers on every new Rational instance in place of the class entry's
 * default (std_object_handlers). Registered as rational_ce_Rational->create_object in
 * rational_operators_minit().
 */
static zend_object *rational_create_object(zend_class_entry *ce)
{
	zend_object *obj = zend_objects_new(ce);
	object_properties_init(obj, ce);
	obj->handlers = &rational_object_handlers;
	return obj;
}
/* }}} */

/* {{{ rational_operators_minit
 *
 * Called from rational_minit() (rational.c), after rational_ce_Rational is registered.
 */
zend_result rational_operators_minit(void)
{
	memcpy(&rational_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	rational_object_handlers.do_operation = rational_do_operation;
	rational_object_handlers.compare = rational_do_compare;
	rational_ce_Rational->create_object = rational_create_object;

	return SUCCESS;
}
/* }}} */
