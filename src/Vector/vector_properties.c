/*
 * vector_properties.c
 *
 * The `magnitude` computed property on OceanMoon\Math\Vector: computed fresh from the current
 * elements on every read, never cached -- mirrors the PHP package's property-hook-based
 * $magnitude (a `get`-only hook with no backing storage), implemented here via a custom
 * `read_property` object handler since internal (C-registered) classes don't support property
 * hooks. Unlike Complex's magnitude/phase (stored, eagerly computed once at construction, since
 * Complex is immutable), Vector is mutable, so caching would need every mutating method (set(),
 * and anything built on it) to remember to invalidate it -- simpler to just never cache.
 *
 * Also installs the custom object handlers (do_operation, create_object) shared by every Vector
 * instance.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "vector_internal.h"
#include "../../oceanmoon_math_arginfo.h"

/* {{{ vector_read_property
 *
 * Custom `read_property` handler: for "magnitude", computes the value fresh from the vector's
 * current elements and returns it directly via `rv`, without touching the (unused) backing
 * property slot. Falls through to zend_std_read_property() for every other property.
 *
 * Deliberately passes NULL for cache_slot on the fallthrough call rather than forwarding the
 * opcode's real cache_slot. The opcode that fetches an object property (e.g. ZEND_FETCH_OBJ_R)
 * uses *cache_slot to remember "this class's property X is at offset N" across repeated
 * executions of the same bytecode location (e.g. a loop body); if populated, the engine's fast
 * path could read the raw property slot directly on a later iteration, bypassing this handler
 * entirely and returning the (permanently unset) backing value instead of a fresh computation --
 * see Complex/complex_properties.c's git history for the same bug, hit and fixed there first.
 */
static zval *vector_read_property(zend_object *object, zend_string *member, int type, void **cache_slot, zval *rv)
{
	if (zend_string_equals_literal(member, "magnitude")) {
		ZVAL_DOUBLE(rv, vector_compute_magnitude(object));
		return rv;
	}

	return zend_std_read_property(object, member, type, NULL, rv);
}
/* }}} */

/* {{{ vector_write_property
 *
 * Custom `write_property` handler: rejects writes to "magnitude" with the same "Property
 * OceanMoon\Math\Vector::$magnitude is read-only" Error the PHP package's `get`-only property
 * hook produces natively -- without this override, the backing slot declared for `magnitude`
 * (needed so the property exists at all for reflection/property_exists()) would silently accept
 * writes that read_property() above then ignores on every subsequent read, a silent no-op instead
 * of the loud failure the package gives. Falls through to zend_std_write_property() for every
 * other property.
 */
static zval *vector_write_property(zend_object *object, zend_string *member, zval *value, void **cache_slot)
{
	if (zend_string_equals_literal(member, "magnitude")) {
		zend_throw_error(NULL, "Property %s::$magnitude is read-only", ZSTR_VAL(object->ce->name));
		return value;
	}

	return zend_std_write_property(object, member, value, cache_slot);
}
/* }}} */

/* The custom object handlers for Vector, installed by vector_create_object(). A copy of the
 * standard handlers with `do_operation`, `read_property`, and `write_property` overridden -- the
 * first for operator support, the other two so `magnitude` behaves like the package's read-only
 * property hook: computed fresh on every read, and rejecting writes, instead of silently
 * accepting them into an ignored backing slot. */
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
	vector_object_handlers.do_operation = vector_do_operation;
	vector_object_handlers.read_property = vector_read_property;
	vector_object_handlers.write_property = vector_write_property;
	vector_ce_Vector->create_object = vector_create_object;

	return SUCCESS;
}
/* }}} */
