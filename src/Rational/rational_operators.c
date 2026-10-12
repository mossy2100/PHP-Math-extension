/*
 * rational_operators.c
 *
 * Operator overloading for OceanMoon\Math\Rational: +, -, *, /, ** (each accepting a Rational,
 * int, or float operand - see below for how a float operand changes the result type), plus
 * unary - (negate), and the comparison operators <, <=, >, >=, <=>, ==, != (all backed by a
 * single `compare` object handler). Mirrors docs/Rational.md in this repo - these operators
 * only exist here in the extension; the userland package has no way to overload operators, so
 * it exposes the same behavior only as named methods (add()/sub()/mul()/div()/pow()/neg()/
 * compare()/etc.). None of those arithmetic methods accepts a float operand (see docs/Rational.md's
 * "Numeric Type Widening" section for why the operators can support float where the methods can't).
 *
 * Numeric widening: when either operand of +, -, *, /, or ** is a genuine float, both operands
 * widen to float and the result is an ordinary float - matching how int widens to float
 * whenever a float is involved in plain PHP arithmetic. When no float is involved, an int
 * operand instead widens to Rational, and the result stays an exact Rational. The one exception
 * is `int ** Rational`, whose int base doesn't widen (see rational_calc_int_pow_rational()).
 *
 * Also owns installing the custom object handlers table (do_operation/compare) on
 * Rational's class entry - unlike Complex, Rational has no lazy computed properties needing
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
#include "../../oceanmoon_math_arginfo.h"
#include "exceptions.h"
#include "floats.h"

/* {{{ rational_operand_to_parts
 *
 * Resolves an operator operand (Rational or int - anything else, including float, is
 * unsupported here) into a (numerator, denominator) pair. Used for the exact-Rational path of
 * +, -, *, / and the `Rational ** int` case of ** - once rational_do_operation() has already
 * established neither operand is a float (see its doc comment), an int operand always widens to
 * Rational exactly, matching add()/sub()/mul()/div()'s own `Rational|int` signatures.
 *
 * Never throws a TypeError for a wrong type - returns FAILURE without an exception, so the
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
 * Resolves an operator operand (Rational, int, or float - anything else is unsupported) to its
 * float value, for the float-widening path of +, -, *, and /. A Rational widens via ordinary
 * numerator/denominator division (matching toFloat(), rational_conversion.c), not through
 * rational_calc_from_float() - that function does the reverse conversion (float -> Rational)
 * and doesn't apply here.
 *
 * Never throws a TypeError for a wrong type - returns FAILURE without an exception, matching
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

/* {{{ rational_check_float_operand
 *
 * Throws DomainException "Invalid operand: <value>. Must be finite." for a non-finite float operand. Like the rest
 * of the library, the float-widened operators never accept or return ±INF or NAN.
 */
static zend_result rational_check_float_operand(zval *zv)
{
	if (Z_TYPE_P(zv) != IS_DOUBLE || zend_finite(Z_DVAL_P(zv))) {
		return SUCCESS;
	}
	zend_string *value_str = math_floats_to_str(Z_DVAL_P(zv));
	zend_string *msg = strpprintf(0, "Invalid operand: %s. Must be finite.", ZSTR_VAL(value_str));
	zend_string_release(value_str);
	zend_throw_exception(spl_ce_DomainException, ZSTR_VAL(msg), 0);
	zend_string_release(msg);
	return FAILURE;
}
/* }}} */

/* {{{ rational_float_result
 *
 * Sets a float operator result, throwing OverflowException "Cannot <operation>. The result would overflow." if it
 * isn't finite. With finite operands, a NAN result means an intermediate value overflowed, so it's reported the same
 * way.
 */
static zend_result rational_float_result(zval *result, double value, const char *operation)
{
	if (!zend_finite(value)) {
		math_throw_overflow(operation);
		return FAILURE;
	}
	ZVAL_DOUBLE(result, value);
	return SUCCESS;
}
/* }}} */

/* {{{ rational_do_float_operation
 *
 * The float-widened path of +, -, *, / for when either operand is a genuine float (see
 * rational_do_operation()'s doc comment). Unlike ordinary float arithmetic, a non-finite operand throws
 * DomainException, an overflowing result throws OverflowException, and division by zero - including float 0.0 -
 * throws ArithmeticException (see docs/Reference/Rational.md).
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
	if (rational_check_float_operand(op1) == FAILURE || rational_check_float_operand(op2) == FAILURE) {
		return FAILURE;
	}

	switch (opcode) {
		case ZEND_ADD:
			return rational_float_result(result, a + b, "add the given Rational and float");
		case ZEND_SUB:
			return rational_float_result(result, a - b, "subtract the given Rational and float");
		case ZEND_MUL:
			return rational_float_result(result, a * b, "multiply the given Rational and float");
		case ZEND_DIV:
			if (b == 0.0) {
				zend_throw_exception(math_ce_ArithmeticException, "Cannot divide by zero.", 0);
				return FAILURE;
			}
			return rational_float_result(result, a / b, "divide the given Rational and float");
		default:
			return FAILURE;
	}
}
/* }}} */

/* {{{ rational_calc_pow_float
 *
 * The `Rational ** float` operator: the float toFloat() ** exp. The caller has checked the exponent is finite.
 * Where C's (and so PHP's) pow() would give NAN (a negative base with a non-integer exponent), this throws
 * DomainException instead, and an overflowing result throws OverflowException. Raising zero to a negative power
 * throws, matching pow() (PHP 8.4 also deprecates 0 ** a negative number).
 */
static zend_result rational_calc_pow_float(zend_long num, zend_long den, double exponent, zval *return_value)
{
	if (num == 0 && exponent < 0) {
		zend_throw_exception(math_ce_ArithmeticException, "Cannot raise zero to negative power.", 0);
		return FAILURE;
	}

	double result = pow((double) num / (double) den, exponent);
	if (zend_isnan(result)) {
		rational_throw_pow_error(spl_ce_DomainException, rational_ratio_to_str(num, den), math_floats_to_str(exponent),
			RATIONAL_POW_NOT_REAL);
		return FAILURE;
	}

	return rational_float_result(return_value, result, "raise the given Rational to the given float power");
}
/* }}} */

/* {{{ rational_calc_float_pow_rational
 *
 * The `float ** Rational` operator, with the Rational exponent p/q in
 * lowest terms. The result is a float, but unlike PHP's own float ** float, a negative base has a
 * real result for an odd root (odd q): (-8.0) ** (1/3) is -2.0, not NAN. The caller has checked the base is finite.
 * An even root of a negative base throws DomainException rather than return NAN, and an overflowing result throws
 * OverflowException. Zero to a negative power also throws, matching pow().
 */
static zend_result rational_calc_float_pow_rational(double base, zend_long p, zend_long q, zval *return_value)
{
	/* Handle a zero base, matching pow(). */
	if (base == 0.0) {
		if (p < 0) {
			zend_throw_exception(math_ce_ArithmeticException, "Cannot raise zero to negative power.", 0);
			return FAILURE;
		}
		ZVAL_DOUBLE(return_value, p == 0 ? 1.0 : 0.0);
		return SUCCESS;
	}

	double exponent = (double) p / (double) q;
	double result;

	if (base < 0) {
		/* A negative base has a real result only for an odd root. */
		if (q % 2 == 0) {
			rational_throw_pow_error(spl_ce_DomainException, math_floats_to_str(base), rational_ratio_to_str(p, q),
				RATIONAL_POW_NOT_REAL);
			return FAILURE;
		}
		/* The odd root of a negative number is the negative of the root of its magnitude, so the
		 * result is negative if p is odd. */
		double magnitude = pow(-base, exponent);
		result = p % 2 == 0 ? magnitude : -magnitude;
	} else {
		result = pow(base, exponent);
	}

	return rational_float_result(return_value, result, "raise the given float to the given Rational power");
}
/* }}} */

/* {{{ rational_do_pow
 *
 * The ** case of rational_do_operation(), split out for readability. Five forms are supported:
 *
 *   1. `Rational ** int`      - exact, via rational_calc_pow(), the same as pow(int).
 *   2. `Rational ** Rational` - exact, via rational_calc_pow_rational(), the same as
 *                               pow(Rational).
 *   3. `int ** Rational`      - exact, via rational_calc_int_pow_rational(), with the same rules.
 *   4. `Rational ** float`    - a float, via rational_calc_pow_float().
 *   5. `float ** Rational`    - a float, via rational_calc_float_pow_rational(), which gives a
 *                               real odd root of a negative base.
 *
 * So, as for the other arithmetic operators, the result is a float if either operand is a float,
 * and otherwise an exact Rational. An exact form throws a DomainException if the result is
 * irrational or not real, rather than falling back to a float. Like the other operators, a float form throws
 * DomainException for a non-finite operand and OverflowException for an overflowing result.
 */
static zend_result rational_do_pow(zval *result, zval *op1, zval *op2)
{
	bool op1_is_rational = Z_TYPE_P(op1) == IS_OBJECT && instanceof_function(Z_OBJCE_P(op1), rational_ce_Rational);
	bool op2_is_rational = Z_TYPE_P(op2) == IS_OBJECT && instanceof_function(Z_OBJCE_P(op2), rational_ce_Rational);

	if (op1_is_rational) {
		zend_long num, den;
		rational_read_parts(Z_OBJ_P(op1), &num, &den);

		if (Z_TYPE_P(op2) == IS_LONG) {
			if (rational_calc_pow(num, den, Z_LVAL_P(op2), result) == FAILURE) {
				math_rethrow_overflow("raise the given Rational to the given power");
				return FAILURE;
			}
			return SUCCESS;
		}
		if (Z_TYPE_P(op2) == IS_DOUBLE) {
			if (rational_check_float_operand(op2) == FAILURE) {
				return FAILURE;
			}
			return rational_calc_pow_float(num, den, Z_DVAL_P(op2), result);
		}
		if (op2_is_rational) {
			zend_long p, q;
			rational_read_parts(Z_OBJ_P(op2), &p, &q);
			if (rational_calc_pow_rational(num, den, p, q, result) == FAILURE) {
				math_rethrow_overflow("raise the given Rational to the given power");
				return FAILURE;
			}
			return SUCCESS;
		}
		return FAILURE;
	}

	if (op2_is_rational) {
		zend_long p, q;
		rational_read_parts(Z_OBJ_P(op2), &p, &q);

		if (Z_TYPE_P(op1) == IS_LONG) {
			if (rational_calc_int_pow_rational(Z_LVAL_P(op1), p, q, result) == FAILURE) {
				math_rethrow_overflow("raise the given int to the given Rational power");
				return FAILURE;
			}
			return SUCCESS;
		}
		if (Z_TYPE_P(op1) == IS_DOUBLE) {
			if (rational_check_float_operand(op1) == FAILURE) {
				return FAILURE;
			}
			return rational_calc_float_pow_rational(Z_DVAL_P(op1), p, q, result);
		}
	}

	return FAILURE;
}
/* }}} */

/* {{{ rational_do_operation
 *
 * The `do_operation` object handler backing Rational's arithmetic operator overloads: +, -, *, /
 * for Rational|int|float on either side, and ** (see rational_do_pow()'s doc comment for its
 * five supported forms). Unary - needs no special-casing here: PHP has no dedicated opcode for
 * it, and the compiler lowers `-$r` to `$r * -1` (see zend_compile_unary_pm() in
 * Zend/zend_compile.c and complex_do_operation()'s doc comment in Complex/complex_operators.c
 * for the fuller explanation), so it's already covered by the ZEND_MUL case. (Unary + similarly
 * lowers to `$r * 1`.)
 *
 * For +, -, *, /: if either operand is a genuine float, both widen to float and
 * rational_do_float_operation() handles it (see docs/Rational.md's "Numeric Type Widening"
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

	/* An overflow is reported with the same message as add()/sub()/mul()/div(). */
	zend_result status;
	const char *operation;
	switch (opcode) {
		case ZEND_ADD:
			status = rational_calc_add_sub(a, b, c, d, false, result);
			operation = "add the given Rational numbers";
			break;
		case ZEND_SUB:
			status = rational_calc_add_sub(a, b, c, d, true, result);
			operation = "subtract the given Rational numbers";
			break;
		case ZEND_MUL:
			status = rational_calc_mul(a, b, c, d, result);
			operation = "multiply the given Rational numbers";
			break;
		case ZEND_DIV:
			if (c == 0) {
				zend_throw_exception(math_ce_ArithmeticException, "Cannot divide by zero.", 0);
				return FAILURE;
			}
			/* (a/b) / (c/d) = (a/b) * (d/c) - reuse rational_calc_mul() with c/d swapped. */
			status = rational_calc_mul(a, b, d, c, result);
			operation = "divide the given Rational numbers";
			break;
		default:
			return FAILURE;
	}

	if (status == FAILURE) {
		math_rethrow_overflow(operation);
	}
	return status;
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
 * instance is actually on (PHP picks op1's handler if op1 is a suitable object, else op2's - see
 * zend_compare()'s IS_OBJECT branch in Zend/zend_operators.c) - so this determines which side is
 * `self` and, if it's op2 (e.g. `5 <=> $r`), negates the result to correct for comparing in the
 * opposite direction from what rational_calc_compare() (self vs. other) computes.
 *
 * Delegates entirely to rational_calc_compare() (rational_comparison.c) - the same core
 * compare()/equal()/etc. use - so operator and method comparisons are guaranteed identical,
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
 * lazy computed properties - Rational has none, so this is the only override it needs). */
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
	rational_object_handlers.cast_object = rational_cast_object;
	rational_ce_Rational->create_object = rational_create_object;

	return SUCCESS;
}
/* }}} */
