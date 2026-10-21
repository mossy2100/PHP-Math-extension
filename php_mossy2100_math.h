/*
 * php_mossy2100_math.h
 *
 * Module header for the Mossy2100 math extension.
 */

#ifndef PHP_MOSSY2100_MATH_H
#define PHP_MOSSY2100_MATH_H

extern zend_module_entry mossy2100_math_module_entry;
#define phpext_mossy2100_math_ptr &mossy2100_math_module_entry

#define PHP_MOSSY2100_MATH_VERSION "4.0.1"

#ifdef ZTS
#include "TSRM.h"
#endif

#endif /* PHP_MOSSY2100_MATH_H */
