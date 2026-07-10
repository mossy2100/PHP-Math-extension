/*
 * complex_internal.h
 *
 * Cross-file declarations shared between complex.c and the other complex_*.c
 * implementation files in this directory. Also exposes complex_minit()/
 * complex_rinit() to ../math.c, which calls them from the module's real
 * PHP_MINIT_FUNCTION/PHP_RINIT_FUNCTION. Not a public API -- nothing here is
 * exposed to other extensions.
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
zend_result complex_to_complex(zval *return_value, zval *value);

/* The custom `compare` object handler, defined in complex_comparison.c. See its doc comment
 * there. Installed onto complex_object_handlers by complex_minit(). */
int complex_compare_objects(zval *op1, zval *op2);

/* Module lifecycle hooks, called from ../math.c. See their doc comments in complex.c. */
zend_result complex_minit(void);
zend_result complex_rinit(int module_number);

#endif /* PHP_COMPLEX_INTERNAL_H */
