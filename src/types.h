/*
 * types.h
 *
 * Shared type-inspection helpers, not specific to any one class - used by Complex's equal()/
 * approxEqual() today, and intended for Rational/Vector/Matrix's equivalents later.
 */

#ifndef PHP_MATH_TYPES_H
#define PHP_MATH_TYPES_H

#include "php.h"

/* {{{ math_types_debug_type_name
 *
 * A narrow equivalent of OceanMoon\Core\Globals\get_debug_type() (PHP's own get_debug_type()),
 * covering only what type-check exception messages need: the scalar/array/null type names, or
 * the class name for an object. Good enough for an exception message - not a general-purpose
 * replacement for get_debug_type().
 */
const char *math_types_debug_type_name(zval *value);
/* }}} */

/* {{{ math_types_mark_read_only
 *
 * Marks the named public properties of ce as `private(set)`: readable from userland, but a write from
 * outside the class raises, matching how the PHP package declares them. gen_stub cannot express
 * asymmetric visibility - given `private(set)` in the stub it emits ZEND_ACC_PUBLIC without a word -
 * so the flag is applied after register_class_*() instead, and the engine then handles both directions
 * correctly: a userland write raises, while the extension's own writes, which go straight to the
 * property slot via math_types_write_property(), are unaffected.
 */
void math_types_mark_read_only(zend_class_entry *ce, const char *const *names, size_t count);
/* }}} */

/* {{{ math_types_property_offset
 *
 * Returns the slot offset of a declared property, for use with math_types_read_property() and
 * math_types_write_property(). Looked up once per property at MINIT, after register_class_*().
 * Reading or writing a property by offset skips everything zend_read_property()/
 * zend_update_property() do by name on every call - allocating and hashing a zend_string for the
 * name, looking up the property info, checking visibility and dispatching through the object's
 * read_property/write_property handlers - which made per-element access in Vector and Matrix
 * slower than the PHP package's own (runtime-cached) property access.
 */
uint32_t math_types_property_offset(zend_class_entry *ce, const char *name);
/* }}} */

/* {{{ math_types_array_iterator
 *
 * Sets return_value to `new ArrayIterator($array)`, for a getIterator() method. Returns FAILURE, with an exception
 * pending, if the constructor throws.
 */
zend_result math_types_array_iterator(zval *return_value, zval *array);
/* }}} */

/* {{{ math_types_clone_obj
 *
 * A `clone_obj` handler that keeps the original's object handlers. PHP's default,
 * zend_objects_clone_obj(), builds the copy with zend_objects_new(), which installs the standard
 * handlers, so a clone would lose the extension's operators, comparison and property handlers.
 */
zend_object *math_types_clone_obj(zend_object *object);
/* }}} */

/* {{{ math_types_throw_unsupported_mod
 *
 * Throws the engine's own "Unsupported operand types" TypeError for `%`, and returns FAILURE. A do_operation handler
 * can't just return FAILURE for `%`, as it does for other unsupported operators: the engine then falls back to
 * converting each object operand to an int, with only a warning.
 */
zend_result math_types_throw_unsupported_mod(zval *op1, zval *op2);
/* }}} */

/* {{{ math_types_do_operation
 *
 * Runs calculate() as a do_operation handler, safely for compound assignment. For `$a += $b` (and the other compound
 * operators) and for `$a++`/`$a--`, the engine passes result and op1 as the same zval, so writing the new value
 * straight into result would overwrite op1's object without releasing it. calculate() writes into a temporary
 * instead, and the old value is released only once it has succeeded, so a failure leaves op1 untouched. This is the
 * same problem ext/gmp's gmp_do_operation() solves.
 */
typedef zend_result (*math_types_operation)(uint8_t opcode, zval *result, zval *op1, zval *op2);

zend_result math_types_do_operation(math_types_operation calculate, uint8_t opcode, zval *result, zval *op1, zval *op2);
/* }}} */

/* {{{ math_types_read_property
 *
 * Returns the zval in a declared property's slot. The caller must already know the object is an
 * instance of the class the offset came from.
 */
static zend_always_inline zval *math_types_read_property(zend_object *obj, uint32_t offset)
{
	zval *slot = OBJ_PROP(obj, offset);
	ZVAL_DEREF(slot);
	return slot;
}
/* }}} */

/* {{{ math_types_write_property
 *
 * Stores value in a declared property's slot, releasing the previous value. Takes ownership of
 * value (no refcount is added), so a freshly built array or object can be moved straight in. Skips
 * the visibility and type checks zend_update_property() would do, so the caller must only ever
 * write a value of the property's declared type.
 */
static zend_always_inline void math_types_write_property(zend_object *obj, uint32_t offset, zval *value)
{
	zval *slot = OBJ_PROP(obj, offset);
	zval_ptr_dtor(slot);
	ZVAL_COPY_VALUE(slot, value);
}
/* }}} */

#endif /* PHP_MATH_TYPES_H */
