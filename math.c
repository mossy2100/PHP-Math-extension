/*
 * math.c
 *
 * Module-wide lifecycle for the `math` extension: MINIT/RINIT/MINFO and the
 * module entry. This is the one extension-wide file; everything class-specific
 * lives in its own subdirectory (Complex/ and Rational/ so far; Vector/, Matrix/
 * will follow the same pattern). MINIT/RINIT delegate to each class's own
 * complex_minit()/complex_rinit()-style hooks rather than doing class-specific
 * work here directly.
 *
 * Shared, non-class-specific infrastructure (floats.c/floats.h, integers.c/integers.h,
 * exceptions.c/exceptions.h) lives flat at this level rather than in a per-class subdirectory --
 * it isn't a ported PHP class, it's support code every class-specific file can depend on. Only
 * exceptions.c needs an MINIT hook (class registration); floats.c/integers.c are pure functions
 * with no PHP-visible state to initialize.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_math.h"
#include "exceptions.h"
#include "Complex/complex_internal.h"
#include "Rational/rational_internal.h"

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(oceanmoon_math)
{
	/* Exception classes are registered first, before any class that might throw them. */
	if (math_exceptions_minit() == FAILURE) {
		return FAILURE;
	}

	if (complex_minit() == FAILURE) {
		return FAILURE;
	}

	if (rational_minit() == FAILURE) {
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(oceanmoon_math)
{
	if (complex_rinit(module_number) == FAILURE) {
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(oceanmoon_math)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "math support", "enabled");
	php_info_print_table_row(2, "version", PHP_MATH_VERSION);
	php_info_print_table_end();
}
/* }}} */

/* {{{ oceanmoon_math_module_entry */
zend_module_entry oceanmoon_math_module_entry = {
	STANDARD_MODULE_HEADER,
	"oceanmoon_math",			/* Extension name */
	NULL,						/* zend_function_entry (no global functions) */
	PHP_MINIT(oceanmoon_math),	/* PHP_MINIT - Module initialization */
	NULL,						/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(oceanmoon_math),	/* PHP_RINIT - Request initialization */
	NULL,						/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(oceanmoon_math),	/* PHP_MINFO - Module info */
	PHP_MATH_VERSION,			/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_OCEANMOON_MATH
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(oceanmoon_math)
#endif
