/*
 * complex_properties.c
 *
 * The `magnitude`/`phase` computed properties on OceanMoon\Math\Complex. Mirrors the PHP
 * package's "Computed properties" region: both are declared nullable, default to null, and are
 * lazily computed and cached in place the first time they're read -- matching the package's
 * property-hook-based caching, implemented here via a custom `read_property` object handler
 * since internal (C-registered) classes don't support property hooks.
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
 * Matches the PHP package's magnitude hook: abs(real) for a real number (imaginary exactly 0),
 * otherwise hypot(real, imaginary).
 */
static double complex_compute_magnitude(zend_object *obj)
{
	double real, imag;
	complex_read_parts(obj, &real, &imag);

	return imag == 0.0 ? fabs(real) : hypot(real, imag);
}
/* }}} */

/* {{{ complex_compute_phase
 *
 * Matches the PHP package's phase hook: for a real number (imaginary exactly 0), PI if negative
 * else 0; otherwise atan2(imaginary, real) wrapped into (-PI, PI] via math_floats_wrap() (mainly
 * to normalize -0.0, since atan2() already returns values in that range).
 */
static double complex_compute_phase(zend_object *obj)
{
	double real, imag;
	complex_read_parts(obj, &real, &imag);

	if (imag == 0.0) {
		return real < 0 ? M_PI : 0.0;
	}

	return math_floats_wrap(atan2(imag, real), 2 * M_PI, true);
}
/* }}} */

/* {{{ complex_read_property
 *
 * Custom `read_property` handler: for "magnitude"/"phase", computes and caches the value (via
 * zend_update_property_double(), so subsequent reads see it already populated) the first time
 * it's read as null, then falls through to zend_std_read_property() either way. Calls
 * zend_std_read_property() directly (not through the object's own handlers) to peek at the
 * current stored value -- going through the handler vtable here would recurse into this same
 * function.
 *
 * Deliberately passes NULL for cache_slot on every zend_std_read_property() call in this function,
 * including the final one -- never the real cache_slot the opcode handed us. The opcode that
 * fetches an object property (e.g. ZEND_FETCH_OBJ_R) uses *cache_slot to remember "this class's
 * property X is at offset N" across repeated executions of the *same* bytecode location (e.g. a
 * loop body). If that cache gets populated here, the engine's fast path can, on the next
 * iteration, read the raw property slot directly -- bypassing this handler (and so our lazy
 * compute-and-cache logic) entirely -- and return the still-uncomputed null. Forcing NULL here
 * means every access genuinely goes through this function, at a small, deliberate cost to the
 * opcode-level fast path for magnitude/phase/real/imaginary alike.
 */
static zval *complex_read_property(zend_object *object, zend_string *member, int type, void **cache_slot, zval *rv)
{
	if (zend_string_equals_literal(member, "magnitude") || zend_string_equals_literal(member, "phase")) {
		zval peek_rv;
		zval *current = zend_std_read_property(object, member, BP_VAR_R, NULL, &peek_rv);

		if (Z_TYPE_P(current) == IS_NULL) {
			double computed = zend_string_equals_literal(member, "magnitude")
				? complex_compute_magnitude(object)
				: complex_compute_phase(object);
			zend_update_property_double(complex_ce_Complex, object, ZSTR_VAL(member), ZSTR_LEN(member), computed);
		}
	}

	return zend_std_read_property(object, member, type, NULL, rv);
}
/* }}} */

/* The custom object handlers for Complex, installed by complex_create_object(). A copy of the
 * standard handlers with `read_property` overridden so magnitude/phase can be lazily computed and
 * cached in place, matching the PHP package's property-hook behavior. */
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
	complex_object_handlers.read_property = complex_read_property;
	complex_ce_Complex->create_object = complex_create_object;

	return SUCCESS;
}
/* }}} */
