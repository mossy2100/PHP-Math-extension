/*
 * complex_internal.h
 *
 * Cross-file declarations shared between complex.c and the other complex_*.c
 * implementation files that make up this module. Not a public API -- nothing
 * here is exposed to other extensions.
 */

#ifndef PHP_COMPLEX_INTERNAL_H
#define PHP_COMPLEX_INTERNAL_H

#include "php.h"

/* The registered class entry for OceanMoon\Math\Complex. Defined in complex.c. */
extern zend_class_entry *complex_ce_Complex;

/* Shared helpers, defined in complex.c. See their doc comments there. */
zend_result complex_init(zend_object *obj, double real, double imag);
zend_result complex_create(zval *return_value, double real, double imag);
zend_result complex_from_array(zval *return_value, HashTable *arr);
zend_result complex_from_object(zval *return_value, zend_object *obj);

#endif /* PHP_COMPLEX_INTERNAL_H */
