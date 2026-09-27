/*
 * oceanmoon_math.c
 *
 * Module-wide lifecycle for the `oceanmoon_math` extension: MINIT/RINIT/MINFO and the module
 * entry. This is the one extension-wide file, kept at the project root alongside
 * php_oceanmoon_math.h per common PHP extension convention; everything else lives under src/,
 * one subdirectory per class (Complex/, Rational/, Vector/, Matrix/). MINIT/RINIT delegate to
 * each class's own complex_minit()/complex_rinit()-style hooks rather than doing class-specific
 * work here directly.
 *
 * Shared, non-class-specific infrastructure (floats.c/floats.h, integers.c/integers.h,
 * exceptions.c/exceptions.h) lives flat at the top of src/ rather than in a per-class
 * subdirectory -- it isn't a ported PHP class, it's support code every class-specific file can
 * depend on. Only exceptions.c needs an MINIT hook (class registration); floats.c/integers.c are
 * pure functions with no PHP-visible state to initialize.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_oceanmoon_math.h"
#include "exceptions.h"
#include "types.h"
#include "Complex/complex_internal.h"
#include "Rational/rational_internal.h"
#include "Vector/vector_internal.h"
#include "Matrix/matrix_internal.h"

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

	if (vector_minit() == FAILURE) {
		return FAILURE;
	}

	if (matrix_minit() == FAILURE) {
		return FAILURE;
	}

	/* Mark the read-only properties of every class: the PHP package declares them private(set), and
	 * gen_stub cannot express asymmetric visibility (it emits ZEND_ACC_PUBLIC for `private(set)` without
	 * a word), so the flags are applied here, after registration. The engine then rejects userland
	 * writes while the extension's own writes, via zend_update_property(ce, ...), keep working. */
	static const char *const complex_read_only[] = { "real", "imaginary", "magnitude", "phase" };
	math_types_mark_read_only(complex_ce_Complex, complex_read_only, sizeof(complex_read_only) / sizeof(complex_read_only[0]));
	static const char *const rational_read_only[] = { "numerator", "denominator" };
	math_types_mark_read_only(rational_ce_Rational, rational_read_only, sizeof(rational_read_only) / sizeof(rational_read_only[0]));
	static const char *const vector_read_only[] = { "count" };
	math_types_mark_read_only(vector_ce_Vector, vector_read_only, sizeof(vector_read_only) / sizeof(vector_read_only[0]));
	static const char *const matrix_read_only[] = { "rowCount", "columnCount" };
	math_types_mark_read_only(matrix_ce_Matrix, matrix_read_only, sizeof(matrix_read_only) / sizeof(matrix_read_only[0]));

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
	php_info_print_table_row(2, "version", PHP_OCEANMOON_MATH_VERSION);
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
	PHP_OCEANMOON_MATH_VERSION,	/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_OCEANMOON_MATH
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(oceanmoon_math)
#endif
