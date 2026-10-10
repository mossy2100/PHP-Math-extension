/*
 * complex_conversion.c
 *
 * Conversion methods for OceanMoon\Math\Complex: __toString(). Mirrors the
 * "Conversion Methods" region of the PHP package's Complex class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <math.h>

#include "php.h"
#include "php_oceanmoon_math.h"
#include "complex_internal.h"
#include "../../oceanmoon_math_arginfo.h"
#include "Zend/zend_smart_str.h"

/* {{{ complex_double_to_str
 *
 * Converts a double to a zend_string the exact same way PHP's own (string) cast does (governed
 * by the `precision` ini setting), by wrapping it in a zval and running it through the engine's
 * normal string-conversion machinery. This gives byte-for-byte parity with the PHP package's
 * __toString(), which itself just does (string) $this->real, etc. - no bespoke formatting logic
 * to keep in sync.
 *
 * Takes ownership of the resulting zend_string; the caller must eventually release it (or hand
 * it to RETURN_STR(), which takes ownership itself).
 */
static zend_string *complex_double_to_str(double value)
{
	zval tmp;
	ZVAL_DOUBLE(&tmp, value);
	convert_to_string(&tmp);
	return Z_STR(tmp);
}
/* }}} */

/* {{{ complex_parts_to_str
 *
 * Formats a complex number from its parts, matching the PHP package's Complex::__toString(). Shared by
 * __toString() and by exception messages that embed a Complex (complex_calc_div() in complex_arithmetic.c).
 *
 * Returns a new zend_string the caller owns and must eventually release.
 */
zend_string *complex_parts_to_str(double real, double imag)
{
	/* Real number: imaginary part is zero. */
	if (imag == 0.0) {
		return complex_double_to_str(real);
	}

	/* Real part is zero, imaginary part non-zero. */
	if (real == 0.0) {
		if (imag == 1.0) {
			return zend_string_init("i", 1, 0);
		}
		if (imag == -1.0) {
			return zend_string_init("-i", 2, 0);
		}

		smart_str buf = {0};
		zend_string *imag_str = complex_double_to_str(imag);
		smart_str_append(&buf, imag_str);
		zend_string_release(imag_str);
		smart_str_appendc(&buf, 'i');
		return smart_str_extract(&buf);
	}

	/* Both parts non-zero: "a + bi" or "a - bi" form. */
	double abs_imag = fabs(imag);

	smart_str buf = {0};
	zend_string *real_str = complex_double_to_str(real);
	smart_str_append(&buf, real_str);
	zend_string_release(real_str);
	smart_str_appends(&buf, imag > 0 ? " + " : " - ");
	if (abs_imag != 1.0) {
		zend_string *imag_str = complex_double_to_str(abs_imag);
		smart_str_append(&buf, imag_str);
		zend_string_release(imag_str);
	}
	smart_str_appendc(&buf, 'i');
	return smart_str_extract(&buf);
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::__toString(): string
 *
 * Matches the PHP package's Complex::__toString().
 */
PHP_METHOD(OceanMoon_Math_Complex, __toString)
{
	zend_object *self = Z_OBJ_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();

	double real, imag;
	complex_read_parts(self, &real, &imag);

	RETURN_STR(complex_parts_to_str(real, imag));
}
/* }}} */
