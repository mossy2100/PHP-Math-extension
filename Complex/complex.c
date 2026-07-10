/*
 * complex.c
 *
 * OceanMoon\Math\Complex: class registration, the constructor, factory
 * methods, and the shared internal helpers (complex_init/complex_create/
 * complex_from_array/complex_from_object) declared in complex_internal.h and
 * used by this file as well as the other complex_*.c implementation files in
 * this directory.
 *
 * complex_minit()/complex_rinit() are called from the module's real
 * PHP_MINIT_FUNCTION/PHP_RINIT_FUNCTION in ../math.c, which owns module
 * lifecycle for the whole `math` extension. Everything in this directory is
 * Complex-specific; rational_*.c/vector_*.c/matrix_*.c (in their own
 * directories) will follow the same pattern once those classes are built.
 *
 * Conversion methods (toArray/toObject/__toString) live in
 * complex_conversion.c; comparison methods (equal/approxEqual) live in
 * complex_comparison.c. No arithmetic, custom object handlers, or operator
 * overloading yet.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"
#include "Zend/zend_constants.h"
#include "php_math.h"
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

/* {{{ complex_is_imaginary_unit */
static bool complex_is_imaginary_unit(char c)
{
	return c == 'i' || c == 'I' || c == 'j' || c == 'J';
}
/* }}} */

/* {{{ complex_match_number
 *
 * Hand-rolled equivalent of the PHP package's `rxNum` regex fragment (unsigned):
 *   (?:\d+\.?\d*|\.\d+)(?:[eE][+-]?\d+)?
 * i.e. digits with an optional trailing ".digits" part, OR a leading "." with required
 * digits -- either way, optionally followed by a well-formed exponent. Tries to match at
 * `pos` in `str` (length `len`, no sign -- the sign is handled by the caller, matching how the
 * PHP pattern captures it in a separate group). On success, sets *out_value to the parsed
 * magnitude and *out_consumed to how many characters matched, and returns true. Deliberately
 * consumes nothing beyond a valid match (e.g. "3e" matches just "3", leaving "e" for the caller
 * to reject) rather than using a general-purpose strtod() over the whole remaining string, which
 * would accept things like "inf"/"nan" that PHP's is_numeric()/this grammar do not.
 */
static bool complex_match_number(const char *str, size_t len, size_t pos, double *out_value, size_t *out_consumed)
{
	size_t start = pos;
	size_t i = pos;

	bool has_digits_before_dot = false;
	while (i < len && str[i] >= '0' && str[i] <= '9') {
		i++;
		has_digits_before_dot = true;
	}

	bool has_dot = false;
	if (i < len && str[i] == '.') {
		has_dot = true;
		i++;
		while (i < len && str[i] >= '0' && str[i] <= '9') {
			i++;
		}
	}

	if (!has_digits_before_dot && (!has_dot || i == start + 1)) {
		/* Neither "digits..." nor ".digits" matched. */
		return false;
	}

	/* Optional exponent: only consumed if followed by at least one digit. */
	if (i < len && (str[i] == 'e' || str[i] == 'E')) {
		size_t j = i + 1;
		if (j < len && (str[j] == '+' || str[j] == '-')) {
			j++;
		}
		size_t digits_start = j;
		while (j < len && str[j] >= '0' && str[j] <= '9') {
			j++;
		}
		if (j > digits_start) {
			i = j;
		}
	}

	size_t match_len = i - start;
	char buf[64];
	if (match_len == 0 || match_len >= sizeof(buf)) {
		return false;
	}
	memcpy(buf, str + start, match_len);
	buf[match_len] = '\0';

	*out_value = strtod(buf, NULL);
	*out_consumed = match_len;
	return true;
}
/* }}} */

/* {{{ complex_is_pure_number
 *
 * Whole-string equivalent of PHP's is_numeric(): an optional leading sign followed by a
 * complex_match_number() match that consumes every remaining character. Rejects "nan"/"inf" the
 * same way complex_match_number() does -- neither contains only digits/dot/exponent characters.
 */
static bool complex_is_pure_number(const char *str, size_t len, double *out_value)
{
	if (len == 0) {
		return false;
	}

	size_t pos = 0;
	double sign = 1.0;
	if (str[0] == '+' || str[0] == '-') {
		sign = (str[0] == '-') ? -1.0 : 1.0;
		pos = 1;
	}

	double value;
	size_t consumed;
	if (!complex_match_number(str, len, pos, &value, &consumed) || pos + consumed != len) {
		return false;
	}

	*out_value = sign * value;
	return true;
}
/* }}} */

/* {{{ complex_try_parse
 *
 * Hand-rolled equivalent of the PHP package's Complex::parse() matching logic (minus whitespace
 * stripping and the empty-string check, both handled by the caller). Tries, in order: a pure
 * real number; a pure imaginary number (optional sign, optional coefficient, i/j suffix); a
 * complex number in "real±imag i" form; a complex number in "imag i±real" form. Returns false if
 * none match.
 */
static bool complex_try_parse(const char *str, size_t len, double *out_real, double *out_imag)
{
	double value;
	if (complex_is_pure_number(str, len, &value)) {
		*out_real = value;
		*out_imag = 0.0;
		return true;
	}

	/* Pure imaginary: [sign]? [coefficient]? i|j (whole string). */
	{
		size_t pos = 0;
		double sign = 1.0;
		if (len > 0 && (str[0] == '+' || str[0] == '-')) {
			sign = (str[0] == '-') ? -1.0 : 1.0;
			pos = 1;
		}

		double coeff = 0.0;
		size_t consumed = 0;
		bool has_coeff = complex_match_number(str, len, pos, &coeff, &consumed);
		size_t after = pos + (has_coeff ? consumed : 0);

		if (after < len && after == len - 1 && complex_is_imaginary_unit(str[after])) {
			*out_real = 0.0;
			*out_imag = sign * (has_coeff ? coeff : 1.0);
			return true;
		}
	}

	/* Complex, real first: [sign]? real (+|-) [coefficient]? i|j (whole string). */
	{
		size_t pos = 0;
		double real_sign = 1.0;
		if (len > 0 && (str[0] == '+' || str[0] == '-')) {
			real_sign = (str[0] == '-') ? -1.0 : 1.0;
			pos = 1;
		}

		double real_val;
		size_t real_consumed;
		if (complex_match_number(str, len, pos, &real_val, &real_consumed)) {
			size_t after_real = pos + real_consumed;
			if (after_real < len && (str[after_real] == '+' || str[after_real] == '-')) {
				double imag_sign = (str[after_real] == '-') ? -1.0 : 1.0;
				size_t imag_pos = after_real + 1;

				double imag_val = 0.0;
				size_t imag_consumed = 0;
				bool has_imag_coeff = complex_match_number(str, len, imag_pos, &imag_val, &imag_consumed);
				size_t after_imag = imag_pos + (has_imag_coeff ? imag_consumed : 0);

				if (after_imag < len && after_imag == len - 1 && complex_is_imaginary_unit(str[after_imag])) {
					*out_real = real_sign * real_val;
					*out_imag = imag_sign * (has_imag_coeff ? imag_val : 1.0);
					return true;
				}
			}
		}
	}

	/* Complex, imaginary first: [sign]? [coefficient]? i|j (+|-) real (whole string). */
	{
		size_t pos = 0;
		double imag_sign = 1.0;
		if (len > 0 && (str[0] == '+' || str[0] == '-')) {
			imag_sign = (str[0] == '-') ? -1.0 : 1.0;
			pos = 1;
		}

		double imag_val = 0.0;
		size_t imag_consumed = 0;
		bool has_imag_coeff = complex_match_number(str, len, pos, &imag_val, &imag_consumed);
		size_t after_imag_coeff = pos + (has_imag_coeff ? imag_consumed : 0);

		if (after_imag_coeff < len && complex_is_imaginary_unit(str[after_imag_coeff])) {
			size_t after_unit = after_imag_coeff + 1;
			if (after_unit < len && (str[after_unit] == '+' || str[after_unit] == '-')) {
				double real_sign = (str[after_unit] == '-') ? -1.0 : 1.0;
				size_t real_pos = after_unit + 1;

				double real_val;
				size_t real_consumed;
				if (complex_match_number(str, len, real_pos, &real_val, &real_consumed)
					&& real_pos + real_consumed == len) {
					*out_imag = imag_sign * (has_imag_coeff ? imag_val : 1.0);
					*out_real = real_sign * real_val;
					return true;
				}
			}
		}
	}

	return false;
}
/* }}} */

/* {{{ complex_parse
 *
 * Shared by parse() and toComplex()'s string branch. Matches the PHP package's Complex::parse(),
 * except the message for an unparseable string throws spl_ce_DomainException directly rather than
 * the package's FormatException (a subclass); the Math test suite only asserts on DomainException,
 * not the more specific type.
 */
static zend_result complex_parse(zval *return_value, zend_string *str)
{
	char *stripped = emalloc(ZSTR_LEN(str) + 1);
	size_t stripped_len = 0;
	for (size_t i = 0; i < ZSTR_LEN(str); i++) {
		char c = ZSTR_VAL(str)[i];
		if (!isspace((unsigned char) c)) {
			stripped[stripped_len++] = c;
		}
	}
	stripped[stripped_len] = '\0';

	if (stripped_len == 0) {
		efree(stripped);
		zend_throw_exception(spl_ce_DomainException, "Cannot parse empty string as complex number.", 0);
		return FAILURE;
	}

	double real, imag;
	bool ok = complex_try_parse(stripped, stripped_len, &real, &imag);

	if (!ok) {
		zend_string *msg = strpprintf(0, "Cannot parse '%s' as complex number.", stripped);
		efree(stripped);
		zend_throw_exception(spl_ce_DomainException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	efree(stripped);

	return complex_create(return_value, real, imag);
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::parse(string $str): Complex */
PHP_METHOD(OceanMoon_Math_Complex, parse)
{
	zend_string *str;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(str)
	ZEND_PARSE_PARAMETERS_END();

	if (complex_parse(return_value, str) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ complex_to_complex
 *
 * Shared by toComplex() and complex_normalize_operand() (complex_comparison.c's shared helper for
 * equal()/approxEqual()), mirroring how the PHP package's equal()/approxEqual() delegate to
 * self::toComplex(). An existing Complex instance is returned unchanged (same instance, not a
 * copy, via RETVAL_ZVAL's refcount bump); everything else is dispatched to the matching from*()/
 * complex_create()/complex_parse() helper, or rejected with InvalidArgumentException.
 */
zend_result complex_to_complex(zval *return_value, zval *value)
{
	if (Z_TYPE_P(value) == IS_OBJECT && instanceof_function(Z_OBJCE_P(value), complex_ce_Complex)) {
		RETVAL_ZVAL(value, 1, 0);
		return SUCCESS;
	}

	if (Z_TYPE_P(value) == IS_LONG || Z_TYPE_P(value) == IS_DOUBLE) {
		return complex_create(return_value, zval_get_double(value), 0.0);
	}

	if (Z_TYPE_P(value) == IS_STRING) {
		return complex_parse(return_value, Z_STR_P(value));
	}

	if (Z_TYPE_P(value) == IS_ARRAY) {
		return complex_from_array(return_value, Z_ARRVAL_P(value));
	}

	if (Z_TYPE_P(value) == IS_OBJECT) {
		return complex_from_object(return_value, Z_OBJ_P(value));
	}

	zend_throw_exception(spl_ce_InvalidArgumentException, "Cannot convert value to Complex. Value must be Complex, int, float, string, array, or object.", 0);
	return FAILURE;
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::toComplex(mixed $value): Complex */
PHP_METHOD(OceanMoon_Math_Complex, toComplex)
{
	zval *value;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	if (complex_to_complex(return_value, value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* The custom object handlers for Complex, installed by complex_create_object(). A copy of the
 * standard handlers with `compare` overridden (complex_comparison.c) so ==/!=/<=> use value
 * comparison instead of the engine's default identity-only behavior for objects. */
static zend_object_handlers complex_object_handlers;

/* {{{ complex_create_object
 *
 * Installs complex_object_handlers on every new Complex instance in place of the class entry's
 * default (std_object_handlers). Registered as complex_ce_Complex->create_object in complex_minit().
 */
static zend_object *complex_create_object(zend_class_entry *ce)
{
	zend_object *obj = zend_objects_new(ce);
	object_properties_init(obj, ce);
	obj->handlers = &complex_object_handlers;
	return obj;
}
/* }}} */

/* {{{ complex_minit
 *
 * Called from PHP_MINIT_FUNCTION(math) in ../math.c. Class entry, method table, and typed
 * properties are all generated from complex.stub.php into complex_arginfo.h; register them here.
 */
zend_result complex_minit(void)
{
	complex_ce_Complex = register_class_OceanMoon_Math_Complex(zend_ce_stringable);

	memcpy(&complex_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	complex_object_handlers.compare = complex_compare_objects;
	complex_ce_Complex->create_object = complex_create_object;

	return SUCCESS;
}
/* }}} */

/* {{{ complex_rinit
 *
 * Called from PHP_RINIT_FUNCTION(math) in ../math.c. Registers OceanMoon\Math\I, a constant
 * Complex(0, 1), fresh on every request.
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
zend_result complex_rinit(int module_number)
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
