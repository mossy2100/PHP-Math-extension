/*
 * complex.c
 *
 * Minimal build-and-load skeleton for the OceanMoon complex extension.
 *
 * Deliberately tiny: it registers OceanMoon\Math\Complex with two properties
 * (real, imaginary), a constructor, and __toString. No math, no custom object
 * handlers, and no operator overloading yet. The goal is only to prove the full
 * toolchain (phpize -> configure -> make -> load) end to end before the real
 * value type and operators are built on top.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"
#include "Zend/zend_constants.h"
#include "php_complex.h"
#include "complex_arginfo.h"

/* The registered class entry for OceanMoon\Math\Complex. */
static zend_class_entry *complex_ce_Complex;

/* {{{ OceanMoon\Math\Complex::__construct(float $real = 0, float $imag = 0) */
PHP_METHOD(OceanMoon_Math_Complex, __construct)
{
	double real = 0.0, imag = 0.0;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_DOUBLE(real)
		Z_PARAM_DOUBLE(imag)
	ZEND_PARSE_PARAMETERS_END();

	/* Matches Complex::__construct()'s own validation in the PHP package. */
	if (!zend_finite(real) || !zend_finite(imag)) {
		zend_throw_exception(spl_ce_DomainException, "Cannot create a complex number from non-finite values.", 0);
		RETURN_THROWS();
	}

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_update_property_double(complex_ce_Complex, self, "real", sizeof("real") - 1, real);
	zend_update_property_double(complex_ce_Complex, self, "imaginary", sizeof("imaginary") - 1, imag);
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::__toString(): string */
PHP_METHOD(OceanMoon_Math_Complex, __toString)
{
	zval rv1, rv2;
	zend_object *self = Z_OBJ_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();

	double real = zval_get_double(
		zend_read_property(complex_ce_Complex, self, "real", sizeof("real") - 1, 1, &rv1));
	double imag = zval_get_double(
		zend_read_property(complex_ce_Complex, self, "imaginary", sizeof("imaginary") - 1, 1, &rv2));

	/* Placeholder formatting; real parity with the PHP __toString() comes later. */
	if (imag == 0.0) {
		RETURN_STR(strpprintf(0, "%g", real));
	}
	RETURN_STR(strpprintf(0, "%g%+gi", real, imag));
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(complex)
{
	/* Class entry, method table, and typed properties are all generated from
	 * complex.stub.php into complex_arginfo.h; register them here. */
	complex_ce_Complex = register_class_OceanMoon_Math_Complex(zend_ce_stringable);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 *
 * Registers OceanMoon\Math\I, a constant Complex(0, 1), fresh on every request.
 *
 * This deliberately runs at RINIT rather than MINIT. MINIT executes once per
 * process, before any request begins; an object built there with the ordinary
 * per-request allocator (object_init_ex) would be destroyed by the engine's
 * end-of-request cleanup after the *first* request, leaving every subsequent
 * request with a dangling reference. RINIT runs at the start of every request,
 * with the per-request heap and object store already active, so the object and
 * the constant name string are properly request-scoped: the engine's standard
 * non-persistent-constant cleanup (triggered by omitting CONST_PERSISTENT from
 * the flags) frees them automatically at request shutdown, and this function
 * rebuilds them fresh on the next request. Same lifetime as a plain PHP
 * `const I = new Complex(0, 1);` declaration re-executed on every request.
 */
PHP_RINIT_FUNCTION(complex)
{
	zend_constant c;

	object_init_ex(&c.value, complex_ce_Complex);
	zend_update_property_double(complex_ce_Complex, Z_OBJ(c.value), "real", sizeof("real") - 1, 0.0);
	zend_update_property_double(complex_ce_Complex, Z_OBJ(c.value), "imaginary", sizeof("imaginary") - 1, 1.0);

	c.name = zend_string_init("OceanMoon\\Math\\I", sizeof("OceanMoon\\Math\\I") - 1, 0);
	c.filename = NULL;
	c.attributes = NULL;
	ZEND_CONSTANT_SET_FLAGS(&c, 0, module_number);

	zend_register_constant(&c);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(complex)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "complex support", "enabled");
	php_info_print_table_row(2, "version", PHP_COMPLEX_VERSION);
	php_info_print_table_end();
}
/* }}} */

/* {{{ complex_module_entry */
zend_module_entry complex_module_entry = {
	STANDARD_MODULE_HEADER,
	"complex",					/* Extension name */
	NULL,						/* zend_function_entry (no global functions) */
	PHP_MINIT(complex),			/* PHP_MINIT - Module initialization */
	NULL,						/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(complex),			/* PHP_RINIT - Request initialization */
	NULL,						/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(complex),			/* PHP_MINFO - Module info */
	PHP_COMPLEX_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_COMPLEX
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(complex)
#endif
