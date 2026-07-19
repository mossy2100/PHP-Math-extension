/*
 * php_math.h
 *
 * Module header for the OceanMoon math extension.
 */

#ifndef PHP_MATH_H
#define PHP_MATH_H

extern zend_module_entry oceanmoon_math_module_entry;
#define phpext_oceanmoon_math_ptr &oceanmoon_math_module_entry

#define PHP_MATH_VERSION "0.1.0"

#ifdef ZTS
#include "TSRM.h"
#endif

#endif /* PHP_MATH_H */
