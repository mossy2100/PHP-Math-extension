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
#include <math.h>
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
#include "exceptions.h"
#include "floats.h"

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

/* {{{ complex_skip_spaces
 *
 * Advances past a run of whitespace characters starting at `pos`, matching a bare `\s*` in the
 * PHP package's regex patterns (used only around the +/- separator between the real and
 * imaginary parts -- everywhere else, e.g. between a coefficient and its i/j suffix, the package's
 * patterns have no \s*, so no whitespace is tolerated there either).
 */
static size_t complex_skip_spaces(const char *str, size_t len, size_t pos)
{
	while (pos < len && isspace((unsigned char) str[pos])) {
		pos++;
	}
	return pos;
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
 * Hand-rolled equivalent of the PHP package's Complex::fromString() matching logic (minus
 * whitespace stripping and the empty-string check, both handled by the caller). Tries, in order:
 * a pure real number; a pure imaginary number (optional sign, optional coefficient, i/j suffix);
 * a complex number in "real±imag i" form; a complex number in "imag i±real" form. Returns false
 * if none match.
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
			size_t after_real = complex_skip_spaces(str, len, pos + real_consumed);
			if (after_real < len && (str[after_real] == '+' || str[after_real] == '-')) {
				double imag_sign = (str[after_real] == '-') ? -1.0 : 1.0;
				size_t imag_pos = complex_skip_spaces(str, len, after_real + 1);

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
			size_t after_unit = complex_skip_spaces(str, len, after_imag_coeff + 1);
			if (after_unit < len && (str[after_unit] == '+' || str[after_unit] == '-')) {
				double real_sign = (str[after_unit] == '-') ? -1.0 : 1.0;
				size_t real_pos = complex_skip_spaces(str, len, after_unit + 1);

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

/* {{{ complex_from_string
 *
 * Shared by fromString() and toComplex()'s string branch. Matches the PHP package's
 * Complex::fromString(): trims leading/trailing whitespace only (internal whitespace is tolerated
 * solely around the +/- separator between real and imaginary parts, handled by
 * complex_try_parse()/complex_skip_spaces() to match the package's regex \s* placement exactly).
 * The message for an unparseable string throws spl_ce_DomainException directly rather than the
 * package's FormatException (a subclass); the Math test suite only asserts on DomainException,
 * not the more specific type.
 */
static zend_result complex_from_string(zval *return_value, zend_string *str)
{
	size_t start = 0;
	while (start < ZSTR_LEN(str) && isspace((unsigned char) ZSTR_VAL(str)[start])) {
		start++;
	}
	size_t end = ZSTR_LEN(str);
	while (end > start && isspace((unsigned char) ZSTR_VAL(str)[end - 1])) {
		end--;
	}
	size_t trimmed_len = end - start;

	char *trimmed = emalloc(trimmed_len + 1);
	memcpy(trimmed, ZSTR_VAL(str) + start, trimmed_len);
	trimmed[trimmed_len] = '\0';

	if (trimmed_len == 0) {
		efree(trimmed);
		zend_throw_exception(math_ce_FormatException, "Cannot convert empty string to Complex.", 0);
		return FAILURE;
	}

	double real, imag;
	bool ok = complex_try_parse(trimmed, trimmed_len, &real, &imag);

	if (!ok) {
		zend_string *msg = strpprintf(0, "Cannot convert string '%s' to Complex. Invalid format.", trimmed);
		efree(trimmed);
		zend_throw_exception(math_ce_FormatException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	efree(trimmed);

	return complex_create(return_value, real, imag);
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::fromString(string $str): Complex */
PHP_METHOD(OceanMoon_Math_Complex, fromString)
{
	zend_string *str;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(str)
	ZEND_PARSE_PARAMETERS_END();

	if (complex_from_string(return_value, str) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::fromPolar(float $mag, float $phase): Complex
 *
 * Matches the PHP package's Complex::fromPolar(): constructs a Complex from polar coordinates
 * (magnitude, phase in radians). The phase is wrapped into (-pi, pi] via math_floats_wrap() before
 * use, mirroring the PHP package's Floats::wrap($phase) call.
 */
PHP_METHOD(OceanMoon_Math_Complex, fromPolar)
{
	double mag, phase;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_DOUBLE(mag)
		Z_PARAM_DOUBLE(phase)
	ZEND_PARSE_PARAMETERS_END();

	if (!zend_finite(mag)) {
		zend_throw_exception(spl_ce_DomainException, "Cannot create Complex. Magnitude must be finite.", 0);
		RETURN_THROWS();
	}
	if (!zend_finite(phase)) {
		zend_throw_exception(spl_ce_DomainException, "Cannot create Complex. Phase must be finite.", 0);
		RETURN_THROWS();
	}
	if (mag < 0) {
		char mag_buf[64];
		snprintf(mag_buf, sizeof(mag_buf), "%g", mag);
		zend_string *msg = strpprintf(0, "Invalid magnitude: %s. Cannot be negative.", mag_buf);
		zend_throw_exception(spl_ce_DomainException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		RETURN_THROWS();
	}

	phase = math_floats_wrap(phase, 2 * M_PI, true);

	if (complex_create(return_value, mag * cos(phase), mag * sin(phase)) == FAILURE) {
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
 * complex_create()/complex_from_string() helper, or rejected with InvalidArgumentException.
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
		return complex_from_string(return_value, Z_STR_P(value));
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

/* {{{ complex_minit
 *
 * Called from PHP_MINIT_FUNCTION(math) in ../math.c. Class entry, method table, and typed
 * properties are all generated from complex.stub.php into complex_arginfo.h; register them here.
 */
zend_result complex_minit(void)
{
	complex_ce_Complex = register_class_OceanMoon_Math_Complex(zend_ce_stringable);

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
	zend_update_property_double(complex_ce_Complex, Z_OBJ(c.value), ZEND_STRL("real"), 0.0);
	zend_update_property_double(complex_ce_Complex, Z_OBJ(c.value), ZEND_STRL("imaginary"), 1.0);

	c.name = zend_string_init("OceanMoon\\Math\\I", sizeof("OceanMoon\\Math\\I") - 1, 0);
	c.filename = NULL;
	c.attributes = NULL;
	ZEND_CONSTANT_SET_FLAGS(&c, 0, module_number);

	zend_register_constant(&c);

	return SUCCESS;
}
/* }}} */
