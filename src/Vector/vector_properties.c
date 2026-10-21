/*
 * vector_properties.c
 *
 * The `magnitude` computed property on OceanMoon\Math\Vector: computed fresh from the current
 * elements on every read, never cached - mirrors the PHP package's property-hook-based
 * $magnitude (a `get`-only hook with no backing storage), implemented here via a custom
 * `read_property` object handler since internal (C-registered) classes don't support property
 * hooks. Unlike Complex's magnitude/phase (stored, eagerly computed once at construction, since
 * Complex is immutable), Vector is mutable, so caching would need every mutating method (set(),
 * and anything built on it) to remember to invalidate it - simpler to just never cache.
 *
 * The has_property and get_properties_for handlers make `magnitude` visible where the package's
 * hook is: isset(), empty(), json_encode(), get_object_vars() and var_export() compute it, while
 * var_dump(), print_r(), (array) and serialize() leave it out.
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
#include "exceptions.h"

/* {{{ vector_read_magnitude
 *
 * Computes the magnitude for the property handlers below, throwing OverflowException, as the package's hook does, if
 * it's too large to represent.
 */
static zend_result vector_read_magnitude(zend_object *object, double *out)
{
	if (vector_compute_magnitude(object, out) == FAILURE) {
		return FAILURE;
	}
	if (!zend_finite(*out)) {
		math_throw_overflow("compute magnitude of the given Vector");
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

/* {{{ vector_read_property
 *
 * Custom `read_property` handler: for "magnitude", computes the value fresh from the vector's
 * current elements and returns it directly via `rv`, without touching the (unused) backing
 * property slot. Falls through to zend_std_read_property() for every other property.
 *
 * The opcode that fetches an object property (e.g. ZEND_FETCH_OBJ_R) uses *cache_slot to remember
 * "this class's property X is at offset N", so later executions of the same bytecode location
 * (e.g. a loop body) read the slot directly without calling this handler. That must never happen
 * for "magnitude": the engine would read its permanently unset backing slot instead of a fresh
 * computation (see Complex/complex_properties.c's git history for the same bug, hit there first).
 * It can't, because "magnitude" returns above without reaching zend_std_read_property(), which is
 * what fills the cache. A cache slot belongs to one opcode with one constant property name, so
 * forwarding it for every other property ($count, and $data within the class) is safe, and keeps
 * userland reads of $v->count as fast as for any plain object.
 */
static zval *vector_read_property(zend_object *object, zend_string *member, int type, void **cache_slot, zval *rv)
{
	if (zend_string_equals_literal(member, "magnitude")) {
		double magnitude;
		if (vector_read_magnitude(object, &magnitude) == FAILURE) {
			return &EG(uninitialized_zval);
		}
		ZVAL_DOUBLE(rv, magnitude);
		return rv;
	}

	return zend_std_read_property(object, member, type, cache_slot, rv);
}
/* }}} */

/* {{{ vector_has_property
 *
 * Custom `has_property` handler: for "magnitude", isset() is true and empty() is true only for a zero magnitude, as
 * with the package's hook, instead of reading the unset backing slot. Like read_property(), it never reaches
 * zend_std_has_property() for "magnitude", so no cache slot is filled for it.
 */
static int vector_has_property(zend_object *object, zend_string *member, int has_set_exists, void **cache_slot)
{
	if (zend_string_equals_literal(member, "magnitude")) {
		if (has_set_exists == ZEND_PROPERTY_EXISTS) {
			return 1;
		}
		double magnitude;
		if (vector_read_magnitude(object, &magnitude) == FAILURE) {
			return 0;
		}
		return has_set_exists == ZEND_PROPERTY_NOT_EMPTY ? magnitude != 0.0 : 1;
	}

	return zend_std_has_property(object, member, has_set_exists, cache_slot);
}
/* }}} */

/* {{{ vector_get_properties_for
 *
 * Custom `get_properties_for` handler: returns the standard property table, with "magnitude" computed for
 * json_encode(), get_object_vars() and var_export(), and left out for every other purpose, matching what the package's
 * hook gives. Without this, those functions skipped "magnitude", and var_dump() showed it as uninitialized.
 */
static HashTable *vector_get_properties_for(zend_object *object, zend_prop_purpose purpose)
{
	bool include = purpose == ZEND_PROP_PURPOSE_JSON || purpose == ZEND_PROP_PURPOSE_GET_OBJECT_VARS
		|| purpose == ZEND_PROP_PURPOSE_VAR_EXPORT;

	double magnitude = 0.0;
	if (include && vector_read_magnitude(object, &magnitude) == FAILURE) {
		return NULL;
	}

	HashTable *props = zend_std_get_properties_for(object, purpose);
	if (props == NULL) {
		return NULL;
	}

	HashTable *result = zend_new_array(zend_hash_num_elements(props));
	zend_ulong index;
	zend_string *key;
	zval *value;
	ZEND_HASH_FOREACH_KEY_VAL(props, index, key, value) {
		if (key != NULL && zend_string_equals_literal(key, "magnitude")) {
			if (include) {
				zval magnitude_zv;
				ZVAL_DOUBLE(&magnitude_zv, magnitude);
				zend_hash_add_new(result, key, &magnitude_zv);
			}
			continue;
		}
		/* A declared property's entry points to its slot (IS_INDIRECT). It's copied as is, rather than dereferenced,
		 * because callers such as get_object_vars() treat a direct value as a dynamic property and skip the
		 * visibility check, which would expose the private $data. */
		Z_TRY_ADDREF_P(value);
		if (key != NULL) {
			zend_hash_add_new(result, key, value);
		} else {
			zend_hash_index_add_new(result, index, value);
		}
	} ZEND_HASH_FOREACH_END();

	zend_release_properties(props);
	return result;
}
/* }}} */

/* {{{ vector_write_property
 *
 * Custom `write_property` handler: rejects writes to "magnitude" with the same "Property
 * OceanMoon\Math\Vector::$magnitude is read-only" Error the PHP package's `get`-only property
 * hook produces natively - without this override, the backing slot declared for `magnitude`
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
 * standard handlers with `do_operation` overridden for operator support, and `read_property`,
 * `write_property`, `has_property` and `get_properties_for` overridden so `magnitude` behaves like
 * the package's read-only property hook: computed fresh on every read, rejecting writes, and
 * visible to the same functions. `clone_obj` is overridden too, so a clone keeps them. */
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
	vector_object_handlers.clone_obj = math_types_clone_obj;
	vector_object_handlers.do_operation = vector_do_operation;
	vector_object_handlers.read_property = vector_read_property;
	vector_object_handlers.write_property = vector_write_property;
	vector_object_handlers.has_property = vector_has_property;
	vector_object_handlers.get_properties_for = vector_get_properties_for;
	vector_ce_Vector->create_object = vector_create_object;

	return SUCCESS;
}
/* }}} */
