/*
 * rational_internal.h
 *
 * Cross-file declarations shared between rational.c and the other rational_*.c implementation
 * files in this directory. Also exposes rational_minit() to ../math.c, which calls it from the
 * module's real PHP_MINIT_FUNCTION. Not a public API -- nothing here is exposed to other
 * extensions.
 */

#ifndef PHP_RATIONAL_INTERNAL_H
#define PHP_RATIONAL_INTERNAL_H

#include "php.h"

/* The registered class entry for OceanMoon\Math\Rational. Defined in rational.c. */
extern zend_class_entry *rational_ce_Rational;

/* Shared helpers, defined in rational.c. See their doc comments there. */
zend_result rational_init(zend_object *obj, zend_long num, zend_long den);
zend_result rational_create(zval *return_value, zend_long num, zend_long den);
void rational_read_parts(zend_object *obj, zend_long *out_num, zend_long *out_den);

/* The computational cores of mul()/inv(), defined in rational_arithmetic.c. Shared with div()
 * (same file), pow() (rational_power.c), and the operators (rational_operators.c) -- see their
 * doc comments there. */
zend_result rational_calc_mul(zend_long a, zend_long b, zend_long c, zend_long d, zval *return_value);
zend_result rational_calc_inv(zend_long num, zend_long den, zval *return_value);

/* The computational core of pow(), defined in rational_power.c. Shared with the ** operator
 * (rational_operators.c). */
zend_result rational_calc_pow(zend_long num, zend_long den, zend_long exponent, zval *return_value);

/* The computational core of fromFloat(), defined in rational_factory.c. Shared with fromString()
 * (same file) and the operators (rational_operators.c, for a float operand). */
zend_result rational_calc_from_float(double value, zval *return_value);

/* The computational core of compare(), defined in rational_comparison.c. Shared with equal()/
 * lessThan()/etc. (same file) and the `compare` operator handler (rational_operators.c). */
zend_result rational_calc_compare(zend_object *self_obj, zval *other, int *out);

/* The `do_operation`/`compare` object handlers backing Rational's operator overloads, and the
 * minit hook that installs them as Rational's custom object handlers table. All defined in
 * rational_operators.c -- called from rational_minit() (rational.c), after
 * rational_ce_Rational is registered. */
zend_result rational_do_operation(uint8_t opcode, zval *result, zval *op1, zval *op2);
int rational_do_compare(zval *op1, zval *op2);
zend_result rational_operators_minit(void);

/* Module lifecycle hook, called from ../math.c. See its doc comment in rational.c. */
zend_result rational_minit(void);

#endif /* PHP_RATIONAL_INTERNAL_H */
