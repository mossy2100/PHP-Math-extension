/*
 * rational_conversion.c
 *
 * Conversion methods for OceanMoon\Math\Rational: toFloat(), toMixedNumber(), __toString().
 * Mirrors the "Conversion methods" region of the PHP package's Rational class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "Zend/zend_smart_str.h"
#include "rational_internal.h"
#include "rational_arginfo.h"

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

/* {{{ OceanMoon\Math\Rational::toMixedNumber(): array
 *
 * Matches the PHP package's Rational::toMixedNumber(): an [integer part, fractional remainder]
 * tuple, using trunc/frac semantics (the remainder carries the same sign as the original). For
 * the proper-fraction case, the PHP source returns `$this` itself (not a copy) as the remainder --
 * replicated here by adding the same zend_object (with an incremented refcount) to the returned
 * array, rather than constructing a new, distinct-but-equal Rational.
 */
PHP_METHOD(OceanMoon_Math_Rational, toMixedNumber)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *self_obj = Z_OBJ_P(ZEND_THIS);
	zend_long num, den;
	rational_read_parts(self_obj, &num, &den);

	array_init(return_value);

	/* If the numerator is 0, the integer part is 0 and the remainder is 0/1. */
	if (num == 0) {
		zval frac;
		if (rational_create(&frac, 0, 1) == FAILURE) {
			zval_ptr_dtor(return_value);
			RETURN_THROWS();
		}
		add_next_index_long(return_value, 0);
		add_next_index_zval(return_value, &frac);
		return;
	}

	/* If the denominator is 1, the integer part is the numerator and the remainder is 0. */
	if (den == 1) {
		zval frac;
		if (rational_create(&frac, 0, 1) == FAILURE) {
			zval_ptr_dtor(return_value);
			RETURN_THROWS();
		}
		add_next_index_long(return_value, num);
		add_next_index_zval(return_value, &frac);
		return;
	}

	/* For proper fractions, the integer part is 0 and the remainder is the original fraction.
	 * -num is always safe here: num is never ZEND_LONG_MIN, by this class's own invariant (see
	 * the Rational::class docblock in the PHP source). */
	zend_long abs_num = num < 0 ? -num : num;
	if (abs_num < den) {
		add_next_index_long(return_value, 0);
		zval self_zv;
		ZVAL_OBJ(&self_zv, self_obj);
		Z_ADDREF(self_zv);
		add_next_index_zval(return_value, &self_zv);
		return;
	}

	/* Calculate the integer part and the remainder. The remainder will have the same sign as the
	 * original fraction -- C's `/`/`%` already truncate toward zero for this, same as PHP's
	 * intdiv()/%. */
	zend_long int_part = num / den;
	zend_long rem = num % den;

	zval frac;
	if (rational_create(&frac, rem, den) == FAILURE) {
		zval_ptr_dtor(return_value);
		RETURN_THROWS();
	}
	add_next_index_long(return_value, int_part);
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
