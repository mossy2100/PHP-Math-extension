/*
 * integers.h
 *
 * Shared overflow-checked integer helpers, not specific to any one class -- ports of
 * OceanMoon\Core\Integers, intended for Rational's arithmetic methods.
 *
 * Unlike the PHP source (which detects overflow for free, by checking whether the engine silently
 * promoted an int expression to float), C has no such promotion: signed integer overflow is
 * undefined behavior, not a value you can observe after the fact. Every function here checks for
 * overflow *before* it can happen, using compiler overflow-checking builtins
 * (__builtin_add_overflow/__builtin_mul_overflow), rather than attempting the operation and
 * inspecting the result.
 */

#ifndef PHP_MATH_INTEGERS_H
#define PHP_MATH_INTEGERS_H

#include <stddef.h>

#include "php.h"

/* {{{ math_integers_result
 *
 * Every function in this file returns one of these instead of throwing directly, matching the
 * "pure C, caller throws" pattern used throughout this file and floats.h -- the caller picks the
 * PHP exception class/message (OverflowException, DomainException, ArgumentCountError; see
 * OceanMoon\Core\Integers' PHP source for which applies to which failure).
 */
typedef enum {
	MATH_INTEGERS_OK,
	/* Result would not fit in a zend_long. add()/mul()/pow()/gcd(). */
	MATH_INTEGERS_OVERFLOW,
	/* pow() only: the exponent was negative. */
	MATH_INTEGERS_NEGATIVE_EXPONENT,
	/* gcd() only: one of the inputs was ZEND_LONG_MIN, which abs() can't safely negate. */
	MATH_INTEGERS_MIN_LONG,
	/* gcd() only: zero arguments were given. */
	MATH_INTEGERS_EMPTY,
} math_integers_result;

/* {{{ math_integers_add
 *
 * Ports OceanMoon\Core\Integers::add(): a + b, with overflow detection.
 */
math_integers_result math_integers_add(zend_long a, zend_long b, zend_long *out);
/* }}} */

/* {{{ math_integers_mul
 *
 * Ports OceanMoon\Core\Integers::mul(): a * b, with overflow detection.
 */
math_integers_result math_integers_mul(zend_long a, zend_long b, zend_long *out);
/* }}} */

/* {{{ math_integers_pow
 *
 * Ports OceanMoon\Core\Integers::pow(): a ** b for a non-negative integer exponent b, with
 * overflow detection. Implemented via exponentiation by squaring using math_integers_mul() at
 * each step (rather than a single native a ** b, which C has no overflow-checked equivalent of),
 * so overflow is caught at the same point the true mathematical result would first exceed the
 * representable range, same as the PHP version.
 */
math_integers_result math_integers_pow(zend_long a, zend_long b, zend_long *out);
/* }}} */

/* {{{ math_integers_gcd
 *
 * Ports OceanMoon\Core\Integers::gcd(): the greatest common divisor of `count` integers via
 * Euclid's algorithm. `nums` must have at least one element (MATH_INTEGERS_EMPTY if count == 0,
 * matching the PHP version's ArgumentCountError); every element must be safely negatable
 * (MATH_INTEGERS_MIN_LONG if any element is ZEND_LONG_MIN, matching the PHP version's
 * DomainException -- abs(ZEND_LONG_MIN) itself overflows, since the type's negative range is one
 * wider than its positive range).
 */
math_integers_result math_integers_gcd(const zend_long *nums, size_t count, zend_long *out);
/* }}} */

#endif /* PHP_MATH_INTEGERS_H */
