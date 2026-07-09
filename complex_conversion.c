/*
 * complex_conversion.c
 *
 * Conversion methods for OceanMoon\Math\Complex: toArray(), toObject(), and
 * __toString(). Mirrors the "Conversion Methods" region of the PHP package's
 * Complex class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <math.h>
#include <string.h>

#include "php.h"
#include "php_complex.h"
#include "complex_internal.h"
#include "complex_arginfo.h"
#include "Zend/zend_smart_str.h"

/* {{{ complex_format_double
 *
 * Mirrors OceanMoon\Core\Floats::format($value)'s defaults (specifier 'g', precision 7,
 * trimZeros true): sprintf with "%.7g", then strip trailing fractional zeros (and a trailing
 * lone "." if the fraction was all zeros).
 *
 * NB: doesn't replicate Floats::format()'s Unicode "×10ⁿ" superscript rendering for scientific
 * notation -- the exponent (if %.7g produces one) is left in sprintf's plain "e+NN" form. No
 * Math test currently asserts on that exact output, only on the fixed-point common case.
 */
static zend_string *complex_format_double(double value)
{
	/* Canonicalize -0.0 to 0.0, matching Floats::normalizeZero(). */
	if (value == 0.0) {
		value = 0.0;
	}

	char buf[64];
	snprintf(buf, sizeof(buf), "%.7g", value);

	char *e_pos = strpbrk(buf, "eE");
	size_t mantissa_len = e_pos ? (size_t) (e_pos - buf) : strlen(buf);

	if (strchr(buf, '.') != NULL) {
		char *end = buf + mantissa_len - 1;
		while (end > buf && *end == '0') {
			*end-- = '\0';
			mantissa_len--;
		}
		if (end > buf && *end == '.') {
			*end = '\0';
			mantissa_len--;
		}
	}

	if (!e_pos) {
		return zend_string_init(buf, mantissa_len, 0);
	}

	smart_str result = {0};
	smart_str_appendl(&result, buf, mantissa_len);
	smart_str_appends(&result, e_pos);
	smart_str_0(&result);
	return result.s;
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::__toString(): string
 *
 * Matches the PHP package's Complex::__toString().
 */
PHP_METHOD(OceanMoon_Math_Complex, __toString)
{
	zval rv1, rv2;
	zend_object *self = Z_OBJ_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();

	double real = zval_get_double(
		zend_read_property(complex_ce_Complex, self, "real", sizeof("real") - 1, 1, &rv1));
	double imag = zval_get_double(
		zend_read_property(complex_ce_Complex, self, "imaginary", sizeof("imaginary") - 1, 1, &rv2));

	/* Real number: imaginary part is zero. */
	if (imag == 0.0) {
		RETURN_STR(complex_format_double(real));
	}

	/* Real part is zero, imaginary part non-zero. */
	if (real == 0.0) {
		if (imag == 1.0) {
			RETURN_STRINGL("i", 1);
		}
		if (imag == -1.0) {
			RETURN_STRINGL("-i", 2);
		}

		smart_str buf = {0};
		zend_string *imag_str = complex_format_double(imag);
		smart_str_append(&buf, imag_str);
		zend_string_release(imag_str);
		smart_str_appendc(&buf, 'i');
		smart_str_0(&buf);
		RETURN_STR(buf.s);
	}

	/* Both parts non-zero: "a + bi" or "a - bi" form. */
	double abs_imag = fabs(imag);

	smart_str buf = {0};
	zend_string *real_str = complex_format_double(real);
	smart_str_append(&buf, real_str);
	zend_string_release(real_str);
	smart_str_appends(&buf, imag > 0 ? " + " : " - ");
	if (abs_imag != 1.0) {
		zend_string *imag_str = complex_format_double(abs_imag);
		smart_str_append(&buf, imag_str);
		zend_string_release(imag_str);
	}
	smart_str_appendc(&buf, 'i');
	smart_str_0(&buf);
	RETURN_STR(buf.s);
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::toArray(): array
 *
 * Matches the PHP package's Complex::toArray(): a list [real, imaginary].
 */
PHP_METHOD(OceanMoon_Math_Complex, toArray)
{
	zval rv1, rv2;
	zend_object *self = Z_OBJ_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();

	double real = zval_get_double(
		zend_read_property(complex_ce_Complex, self, "real", sizeof("real") - 1, 1, &rv1));
	double imag = zval_get_double(
		zend_read_property(complex_ce_Complex, self, "imaginary", sizeof("imaginary") - 1, 1, &rv2));

	array_init(return_value);
	add_next_index_double(return_value, real);
	add_next_index_double(return_value, imag);
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::toObject(): stdClass
 *
 * Matches the PHP package's Complex::toObject(): a plain stdClass with "real"
 * and "imaginary" properties.
 */
PHP_METHOD(OceanMoon_Math_Complex, toObject)
{
	zval rv1, rv2;
	zend_object *self = Z_OBJ_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();

	double real = zval_get_double(
		zend_read_property(complex_ce_Complex, self, "real", sizeof("real") - 1, 1, &rv1));
	double imag = zval_get_double(
		zend_read_property(complex_ce_Complex, self, "imaginary", sizeof("imaginary") - 1, 1, &rv2));

	object_init(return_value);
	add_property_double(return_value, "real", real);
	add_property_double(return_value, "imaginary", imag);
}
/* }}} */
