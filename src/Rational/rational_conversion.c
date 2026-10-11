/*
 * rational_conversion.c
 *
 * Conversion methods for OceanMoon\Math\Rational: toFloat(), toMixedNumber(), __toString().
 * Mirrors the "Conversion methods" region of the PHP package's Rational class. Also the `cast_object`
 * handler behind `(float) $r` and `(int) $r`, which only exist in the extension.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "Zend/zend_smart_str.h"
#include "rational_internal.h"
#include "../../oceanmoon_math_arginfo.h"

/* {{{ OceanMoon\Math\Rational::toFloat(): float
 *
 * Matches the PHP package's Rational::toFloat(): numerator / denominator.
 */
PHP_METHOD(OceanMoon_Math_Rational, toFloat)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_long num, den;
	rational_read_parts(Z_OBJ_P(ZEND_THIS), &num, &den);

	RETURN_DOUBLE((double) num / (double) den);
}
/* }}} */

/* {{{ rational_cast_object
 *
 * The `cast_object` object handler, so `(float) $r` gives the same value as toFloat(), and `(int) $r`
 * the same as trunc(), as `(int)` truncates a float. floatval(), intval() and settype() use the same
 * cast. Every other cast goes to the standard handler: `(string)` calls __toString(), and `(bool)` is
 * true, as for any object.
 */
zend_result rational_cast_object(zend_object *obj, zval *result, int type)
{
	if (type == IS_DOUBLE || type == IS_LONG) {
		zend_long num, den;
		rational_read_parts(obj, &num, &den);
		if (type == IS_DOUBLE) {
			ZVAL_DOUBLE(result, (double) num / (double) den);
		} else {
			ZVAL_LONG(result, num / den);
		}
		return SUCCESS;
	}

	return zend_std_cast_object_tostring(obj, result, type);
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::toMixedNumber(): array
 *
 * Matches the PHP package's Rational::toMixedNumber(): an [integer part, fractional remainder]
 * tuple, using trunc/frac semantics. C's `/` and `%` truncate toward zero, like PHP's intdiv() and
 * `%`, so the remainder has the same sign as the original.
 */
PHP_METHOD(OceanMoon_Math_Rational, toMixedNumber)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_long num, den;
	rational_read_parts(Z_OBJ_P(ZEND_THIS), &num, &den);

	zval frac;
	if (rational_create(&frac, num % den, den) == FAILURE) {
		RETURN_THROWS();
	}

	array_init(return_value);
	add_next_index_long(return_value, num / den);
	add_next_index_zval(return_value, &frac);
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::__toString(): string
 *
 * Matches the PHP package's Rational::__toString(): "numerator" if denominator is 1, otherwise
 * "numerator/denominator".
 */
PHP_METHOD(OceanMoon_Math_Rational, __toString)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_long num, den;
	rational_read_parts(Z_OBJ_P(ZEND_THIS), &num, &den);

	smart_str buf = {0};
	smart_str_append_long(&buf, num);
	if (den != 1) {
		smart_str_appendc(&buf, '/');
		smart_str_append_long(&buf, den);
	}
	smart_str_0(&buf);
	RETURN_STR(buf.s);
}
/* }}} */
