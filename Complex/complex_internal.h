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
void complex_read_parts(zend_object *obj, double *out_real, double *out_imag);

/* Installs the magnitude/phase computed-property object handlers. Defined in
 * complex_properties.c; called from complex_minit() (complex.c). */
zend_result complex_properties_minit(void);

/* Module lifecycle hooks, called from ../math.c. See their doc comments in complex.c. */
zend_result complex_minit(void);
zend_result complex_rinit(int module_number);

#endif /* PHP_COMPLEX_INTERNAL_H */
