/*
 * complex_properties.c
 *
 * Computes the `magnitude`/`phase` properties on OceanMoon\Math\Complex, and installs the custom
 * object handlers (do_operation, compare, create_object) shared by every Complex instance.
 *
 * magnitude/phase are plain stored properties, eagerly computed once in complex_init()
 * (complex.c) at construction time -- Complex is immutable, so there's nothing to keep them in
 * sync with afterward. This matches the PHP package's constructor, which computes both directly
 * rather than via a lazy property hook.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <math.h>

#include "php.h"
#include "php_math.h"
#include "complex_internal.h"
#include "complex_arginfo.h"
#include "floats.h"

/* {{{ complex_compute_magnitude
 *
 * Matches the PHP package's magnitude computation: abs(real) for a real number (imaginary exactly
 * 0), otherwise hypot(real, imaginary).
 */
double complex_compute_magnitude(double real, double imag)
{
	return imag == 0.0 ? fabs(real) : hypot(real, imag);
}
/* }}} */

/* {{{ complex_compute_phase
 *
 * Matches the PHP package's phase computation: for a real number (imaginary exactly 0), PI if
 * negative else 0; otherwise atan2(imaginary, real) wrapped into (-PI, PI] via math_floats_wrap()
 * (mainly to normalize -0.0, since atan2() already returns values in that range).
 */
double complex_compute_phase(double real, double imag)
{
	if (imag == 0.0) {
		return real < 0 ? M_PI : 0.0;
	}

	return math_floats_wrap(atan2(imag, real), 2 * M_PI, true);
}
/* }}} */

/* The custom object handlers for Complex, installed by complex_create_object(). A copy of the
 * standard handlers with `do_operation`/`compare` overridden for operator support. */
static zend_object_handlers complex_object_handlers;

/* {{{ complex_create_object
 *
 * Installs complex_object_handlers on every new Complex instance in place of the class entry's
 * default (std_object_handlers). Registered as complex_ce_Complex->create_object in
 * complex_properties_minit().
 */
static zend_object *complex_create_object(zend_class_entry *ce)
{
	zend_object *obj = zend_objects_new(ce);
	object_properties_init(obj, ce);
	obj->handlers = &complex_object_handlers;
	return obj;
}
/* }}} */

/* {{{ complex_properties_minit
 *
 * Called from complex_minit() (complex.c), after complex_ce_Complex is registered.
 */
zend_result complex_properties_minit(void)
{
	memcpy(&complex_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	complex_object_handlers.do_operation = complex_do_operation;
	complex_object_handlers.compare = complex_do_compare;
	complex_ce_Complex->create_object = complex_create_object;

	return SUCCESS;
}
/* }}} */
