/*
 * math.c
 *
 * Module-wide lifecycle for the `math` extension: MINIT/RINIT/MINFO and the
 * module entry. This is the one extension-wide file; everything class-specific
 * lives in its own subdirectory (Complex/ so far; Rational/, Vector/, Matrix/
 * will follow the same pattern). MINIT/RINIT delegate to each class's own
 * complex_minit()/complex_rinit()-style hooks rather than doing class-specific
 * work here directly.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_math.h"
#include "Complex/complex_internal.h"

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(math)
{
	if (complex_minit() == FAILURE) {
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(math)
{
	if (complex_rinit(module_number) == FAILURE) {
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(math)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "math support", "enabled");
	php_info_print_table_row(2, "version", PHP_MATH_VERSION);
	php_info_print_table_end();
}
/* }}} */

/* {{{ math_module_entry */
zend_module_entry math_module_entry = {
	STANDARD_MODULE_HEADER,
	"math",						/* Extension name */
	NULL,						/* zend_function_entry (no global functions) */
	PHP_MINIT(math),			/* PHP_MINIT - Module initialization */
	NULL,						/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(math),			/* PHP_RINIT - Request initialization */
	NULL,						/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(math),			/* PHP_MINFO - Module info */
	PHP_MATH_VERSION,			/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_MATH
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(math)
#endif
