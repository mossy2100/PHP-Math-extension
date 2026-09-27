/*
 * types.h
 *
 * Shared type-inspection helpers, not specific to any one class -- used by Complex's equal()/
 * approxEqual() today, and intended for Rational/Vector/Matrix's equivalents later.
 */

#ifndef PHP_MATH_TYPES_H
#define PHP_MATH_TYPES_H

#include "php.h"

/* {{{ math_types_debug_type_name
 *
 * A narrow equivalent of OceanMoon\Core\Globals\get_debug_type() (PHP's own get_debug_type()),
 * covering only what type-check exception messages need: the scalar/array/null type names, or
 * the class name for an object. Good enough for an exception message -- not a general-purpose
 * replacement for get_debug_type().
 */
const char *math_types_debug_type_name(zval *value);
/* }}} */

/* {{{ math_types_mark_read_only
 *
 * Marks the named public properties of ce as `private(set)`: readable from userland, but a write from
 * outside the class raises, matching how the PHP package declares them. gen_stub cannot express
 * asymmetric visibility -- given `private(set)` in the stub it emits ZEND_ACC_PUBLIC without a word --
 * so the flag is applied after register_class_*() instead, and the engine then handles both directions
 * correctly: a userland write raises, while the extension's own writes via zend_update_property(ce, ...)
 * keep working. That is the same mechanism that already lets the extension write the private `data`
 * slots on Vector and Matrix from C.
 */
void math_types_mark_read_only(zend_class_entry *ce, const char *const *names, size_t count);
/* }}} */

#endif /* PHP_MATH_TYPES_H */
