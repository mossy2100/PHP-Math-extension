/*
 * vector_properties.c
 *
 * The `magnitude` computed property on OceanMoon\Math\Vector. Mirrors the PHP package's
 * "Computed properties" region: declared nullable, defaults to null, and is lazily computed and
 * cached in place the first time it's read, invalidated back to null by vector_write_element()
 * (vector.c) on every write -- matching the package's property-hook-based caching, implemented
 * here via a custom `read_property` object handler since internal (C-registered) classes don't
 * support property hooks. Directly mirrors Complex/complex_properties.c's magnitude/phase
 * pattern (including its opcode-level inline-property-cache workaround, explained below).
 *
 * Also owns installing the custom object handlers table (read_property) on Vector's class entry.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <math.h>
#include <string.h>

#include "php.h"
#include "vector_internal.h"
#include "vector_arginfo.h"

/* {{{ vector_compute_magnitude
 *
 * Matches the PHP package's magnitude hook: sqrt(sum of squares).
 */
static double vector_compute_magnitude(zend_object *obj)
{
	zend_long size = vector_read_size(obj);
	double sum = 0.0;

	for (zend_long i = 0; i < size; i++) {
		double element;
		/* Bounds are trivially satisfied (0..size-1), so this never fails. */
		vector_read_element(obj, i, &element);
		sum += element * element;
	}

	return sqrt(sum);
}
/* }}} */

/* {{{ vector_read_property
 *
 * Custom `read_property` handler: for "magnitude", computes and caches the value (via
 * zend_update_property_double(), so subsequent reads see it already populated) the first time
 * it's read as null, then falls through to zend_std_read_property() either way. Calls
 * zend_std_read_property() directly (not through the object's own handlers) to peek at the
 * current stored value -- going through the handler vtable here would recurse into this same
 * function.
 *
 * Deliberately passes NULL for cache_slot on every zend_std_read_property() call in this
 * function, including the final one -- never the real cache_slot the opcode handed us. The
 * opcode that fetches an object property (e.g. ZEND_FETCH_OBJ_R) uses *cache_slot to remember
 * "this class's property X is at offset N" across repeated executions of the *same* bytecode
 * location (e.g. a loop body). If that cache gets populated here, the engine's fast path can, on
 * the next iteration, read the raw property slot directly -- bypassing this handler (and so our
 * lazy compute-and-cache logic) entirely -- and return the still-uncomputed null. Forcing NULL
 * here means every access genuinely goes through this function, at a small, deliberate cost to
 * the opcode-level fast path for magnitude/data/size alike.
 */
static zval *vector_read_property(zend_object *object, zend_string *member, int type, void **cache_slot, zval *rv)
{
	if (zend_string_equals_literal(member, "magnitude")) {
		zval peek_rv;
		zval *current = zend_std_read_property(object, member, BP_VAR_R, NULL, &peek_rv);

		if (Z_TYPE_P(current) == IS_NULL) {
			double computed = vector_compute_magnitude(object);
			zend_update_property_double(vector_ce_Vector, object, ZSTR_VAL(member), ZSTR_LEN(member), computed);
		}
	}

	return zend_std_read_property(object, member, type, NULL, rv);
}
/* }}} */

/* The custom object handlers for Vector, installed by vector_create_object(). A copy of the
 * standard handlers with `read_property` overridden so magnitude can be lazily computed and
 * cached in place, matching the PHP package's property-hook behavior. */
static zend_object_handlers vector_object_handlers;

/* {{{ vector_create_object
 *
 * Installs vector_object_handlers on every new Vector instance in place of the class entry's
 * default (std_object_handlers). Registered as vector_ce_Vector->create_object in
 * vector_properties_minit().
 */
static zend_object *vector_create_object(zend_class_entry *ce)
{
	zend_object *obj = zend_objects_new(ce);
	object_properties_init(obj, ce);
	obj->handlers = &vector_object_handlers;
	return obj;
}
/* }}} */

/* {{{ vector_properties_minit
 *
 * Called from vector_minit() (vector.c), after vector_ce_Vector is registered.
 */
zend_result vector_properties_minit(void)
{
	memcpy(&vector_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	vector_object_handlers.read_property = vector_read_property;
	vector_ce_Vector->create_object = vector_create_object;

	return SUCCESS;
}
/* }}} */
