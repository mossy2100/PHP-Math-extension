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

#endif /* PHP_MATH_TYPES_H */
