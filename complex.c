/*
 * complex.c
 *
 * Core of the OceanMoon complex extension: class registration (MINIT/RINIT),
 * the constructor, the module entry, and the shared internal helpers
 * (complex_init/complex_create/complex_from_array/complex_from_object)
 * declared in complex_internal.h and used by this file's factory methods as
 * well as the other complex_*.c implementation files.
 *
 * Conversion methods (toArray/toObject/__toString) live in
 * complex_conversion.c. No arithmetic, custom object handlers, or operator
 * overloading yet.
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
#include "complex_internal.h"
#include "complex_arginfo.h"

/* The registered class entry for OceanMoon\Math\Complex. */
zend_class_entry *complex_ce_Complex;

/* {{{ complex_init
 *
 * Validate real/imaginary and set them on an already-allocated Complex object. Shared by
 * __construct and complex_create() (used by the from*()/toComplex() factory methods), mirroring
 * how the PHP implementation's factory methods all funnel through `new self(...)`, so the
 * finite-value check lives in exactly one place.
 *
 * Returns FAILURE (with an exception already thrown) if either value is non-finite.
 */
zend_result complex_init(zend_object *obj, double real, double imag)
{
	if (!zend_finite(real) || !zend_finite(imag)) {
		zend_throw_exception(spl_ce_DomainException, "Cannot create a complex number from non-finite values.", 0);
		return FAILURE;
	}

	zend_update_property_double(complex_ce_Complex, obj, "real", sizeof("real") - 1, real);
	zend_update_property_double(complex_ce_Complex, obj, "imaginary", sizeof("imaginary") - 1, imag);

	return SUCCESS;
}
/* }}} */

/* {{{ complex_create
 *
 * Allocate a new Complex object into return_value and initialize it via complex_init(). Used by
 * all the from*()/toComplex() factory paths.
 */
zend_result complex_create(zval *return_value, double real, double imag)
{
	object_init_ex(return_value, complex_ce_Complex);
	return complex_init(Z_OBJ_P(return_value), real, imag);
}
/* }}} */

/* {{{ complex_from_array
 *
 * Shared by fromArray() and toComplex()'s array branch, matching the PHP package's
 * Complex::fromArray(). Accepts two shapes:
 *  - A list: [real, imaginary] -- must contain exactly two numeric (int or float) elements.
 *  - An associative array: ['real' => ..., 'imaginary' => ...] -- key order doesn't matter, and
 *    extra keys (e.g. 'magnitude'/'phase' from (array) $complex) are ignored.
 */
zend_result complex_from_array(zval *return_value, HashTable *arr)
{
	if (zend_array_is_list(arr)) {
		if (zend_hash_num_elements(arr) != 2) {
			zend_throw_exception(spl_ce_LengthException, "Cannot convert array to Complex. Array must contain exactly two elements.", 0);
			return FAILURE;
		}

		zval *real_zv = zend_hash_index_find(arr, 0);
		zval *imag_zv = zend_hash_index_find(arr, 1);

		if (Z_TYPE_P(real_zv) != IS_LONG && Z_TYPE_P(real_zv) != IS_DOUBLE) {
			zend_throw_exception(spl_ce_DomainException, "Cannot convert array to Complex. Element at index 0 must be numeric (int or float).", 0);
			return FAILURE;
		}
		if (Z_TYPE_P(imag_zv) != IS_LONG && Z_TYPE_P(imag_zv) != IS_DOUBLE) {
			zend_throw_exception(spl_ce_DomainException, "Cannot convert array to Complex. Element at index 1 must be numeric (int or float).", 0);
			return FAILURE;
		}

		return complex_create(return_value, zval_get_double(real_zv), zval_get_double(imag_zv));
	}

	zval *real_zv = zend_hash_str_find(arr, "real", sizeof("real") - 1);
	if (!real_zv) {
		zend_throw_exception(spl_ce_DomainException, "Cannot convert array to Complex. Array is missing key \"real\".", 0);
		return FAILURE;
	}
	zval *imag_zv = zend_hash_str_find(arr, "imaginary", sizeof("imaginary") - 1);
	if (!imag_zv) {
		zend_throw_exception(spl_ce_DomainException, "Cannot convert array to Complex. Array is missing key \"imaginary\".", 0);
		return FAILURE;
	}

	if (Z_TYPE_P(real_zv) != IS_LONG && Z_TYPE_P(real_zv) != IS_DOUBLE) {
		zend_throw_exception(spl_ce_DomainException, "Cannot convert array to Complex. Value for key \"real\" must be numeric (int or float).", 0);
		return FAILURE;
	}
	if (Z_TYPE_P(imag_zv) != IS_LONG && Z_TYPE_P(imag_zv) != IS_DOUBLE) {
		zend_throw_exception(spl_ce_DomainException, "Cannot convert array to Complex. Value for key \"imaginary\" must be numeric (int or float).", 0);
		return FAILURE;
	}

	return complex_create(return_value, zval_get_double(real_zv), zval_get_double(imag_zv));
}
/* }}} */

/* {{{ complex_from_object
 *
 * Shared by fromObject() and toComplex()'s object branch. $obj must have numeric (int or float)
 * "real" and "imaginary" properties -- matching the PHP package's Complex::fromObject().
 */
zend_result complex_from_object(zval *return_value, zend_object *obj)
{
	zend_string *real_str = zend_string_init("real", sizeof("real") - 1, 0);
	bool has_real = obj->handlers->has_property(obj, real_str, ZEND_PROPERTY_EXISTS, NULL);
	zend_string_release(real_str);
	if (!has_real) {
		zend_throw_exception(spl_ce_DomainException, "Cannot convert object to Complex. Object is missing property \"real\".", 0);
		return FAILURE;
	}

	zend_string *imag_str = zend_string_init("imaginary", sizeof("imaginary") - 1, 0);
	bool has_imag = obj->handlers->has_property(obj, imag_str, ZEND_PROPERTY_EXISTS, NULL);
	zend_string_release(imag_str);
	if (!has_imag) {
		zend_throw_exception(spl_ce_DomainException, "Cannot convert object to Complex. Object is missing property \"imaginary\".", 0);
		return FAILURE;
	}

	zval rv1, rv2;
	zval *real_zv = zend_read_property(obj->ce, obj, "real", sizeof("real") - 1, /* silent */ true, &rv1);
	zval *imag_zv = zend_read_property(obj->ce, obj, "imaginary", sizeof("imaginary") - 1, /* silent */ true, &rv2);

	if (Z_TYPE_P(real_zv) != IS_LONG && Z_TYPE_P(real_zv) != IS_DOUBLE) {
		zend_throw_exception(spl_ce_DomainException, "Cannot convert object to Complex. Value for property \"real\" must be numeric (int or float).", 0);
		return FAILURE;
	}
	if (Z_TYPE_P(imag_zv) != IS_LONG && Z_TYPE_P(imag_zv) != IS_DOUBLE) {
		zend_throw_exception(spl_ce_DomainException, "Cannot convert object to Complex. Value for property \"imaginary\" must be numeric (int or float).", 0);
		return FAILURE;
	}

	return complex_create(return_value, zval_get_double(real_zv), zval_get_double(imag_zv));
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::__construct(float $real = 0, float $imag = 0) */
PHP_METHOD(OceanMoon_Math_Complex, __construct)
{
	double real = 0.0, imag = 0.0;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_DOUBLE(real)
		Z_PARAM_DOUBLE(imag)
	ZEND_PARSE_PARAMETERS_END();

	if (complex_init(Z_OBJ_P(ZEND_THIS), real, imag) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::fromArray(array $arr): Complex */
PHP_METHOD(OceanMoon_Math_Complex, fromArray)
{
	zval *arr;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(arr)
	ZEND_PARSE_PARAMETERS_END();

	if (complex_from_array(return_value, Z_ARRVAL_P(arr)) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::fromObject(object $obj): Complex */
PHP_METHOD(OceanMoon_Math_Complex, fromObject)
{
	zval *obj;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT(obj)
	ZEND_PARSE_PARAMETERS_END();

	if (complex_from_object(return_value, Z_OBJ_P(obj)) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::toComplex(mixed $value): Complex
 *
 * NB: unlike the PHP package's toComplex(), string values aren't handled yet (parse() isn't
 * implemented in the extension), so a string currently falls through to the final
 * InvalidArgumentException branch rather than being parsed.
 */
PHP_METHOD(OceanMoon_Math_Complex, toComplex)
{
	zval *value;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	/* Existing Complex instance: return unchanged (same instance, not a copy). */
	if (Z_TYPE_P(value) == IS_OBJECT && instanceof_function(Z_OBJCE_P(value), complex_ce_Complex)) {
		RETURN_ZVAL(value, 1, 0);
	}

	/* int or float: real Complex with zero imaginary part. */
	if (Z_TYPE_P(value) == IS_LONG || Z_TYPE_P(value) == IS_DOUBLE) {
		if (complex_create(return_value, zval_get_double(value), 0.0) == FAILURE) {
			RETURN_THROWS();
		}
		return;
	}

	if (Z_TYPE_P(value) == IS_ARRAY) {
		if (complex_from_array(return_value, Z_ARRVAL_P(value)) == FAILURE) {
			RETURN_THROWS();
		}
		return;
	}

	if (Z_TYPE_P(value) == IS_OBJECT) {
		if (complex_from_object(return_value, Z_OBJ_P(value)) == FAILURE) {
			RETURN_THROWS();
		}
		return;
	}

	zend_throw_exception(spl_ce_InvalidArgumentException, "Cannot convert value to Complex. Value must be Complex, int, float, array, or object.", 0);
	RETURN_THROWS();
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
