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
zend_result complex_from_polar(zval *return_value, double mag, double phase);
void complex_read_parts(zend_object *obj, double *out_real, double *out_imag);
void complex_read_magnitude_phase(zend_object *obj, double *out_magnitude, double *out_phase);

/* Installs the magnitude/phase computed-property object handlers. Defined in
 * complex_properties.c; called from complex_minit() (complex.c). */
zend_result complex_properties_minit(void);

/* Shared by add()/sub()/mul()/div() (complex_arithmetic.c) and pow() (complex_power.c). See its
 * doc comment in complex_arithmetic.c. */
zend_result complex_read_other(zval *other, double *out_real, double *out_imag);

/* The computational core of exp()/ln() (complex_power.c's pow() calls these directly, without
 * going through the PHP methods, to reuse their shortcuts). See their doc comments in
 * complex_transcendental.c. */
void complex_calc_exp(double real, double imag, double *out_real, double *out_imag);
zend_result complex_calc_ln(zend_object *obj, double *out_real, double *out_imag);

/* Module lifecycle hooks, called from ../math.c. See their doc comments in complex.c. */
zend_result complex_minit(void);
zend_result complex_rinit(int module_number);

#endif /* PHP_COMPLEX_INTERNAL_H */
