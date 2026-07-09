/*
 * php_complex.h
 *
 * Module header for the OceanMoon complex extension.
 */

#ifndef PHP_COMPLEX_H
#define PHP_COMPLEX_H

extern zend_module_entry complex_module_entry;
#define phpext_complex_ptr &complex_module_entry

#define PHP_COMPLEX_VERSION "0.1.0"

#ifdef ZTS
#include "TSRM.h"
#endif

#endif /* PHP_COMPLEX_H */
