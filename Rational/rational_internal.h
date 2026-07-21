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

/* Module lifecycle hook, called from ../math.c. See its doc comment in rational.c. */
zend_result rational_minit(void);

#endif /* PHP_RATIONAL_INTERNAL_H */
