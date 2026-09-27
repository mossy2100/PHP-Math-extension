/*
 * rational_inspection.c
 *
 * Inspection methods for OceanMoon\Math\Rational: isInt(), sign(). Mirrors the "Inspection methods"
 * region of the PHP package's Rational class, where sign() delegates to Numbers::sign() -- the same
 * helper this extension ports to src/integers.c for the integer case.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "rational_internal.h"
#include "../../oceanmoon_math_arginfo.h"

/* {{{ OceanMoon\Math\Rational::isInt(): bool
 *
 * Matches the PHP package's Rational::isInt(): a denominator of 1 means the value is a whole number.
 * Rationals are always stored in canonical, reduced form (see rational_init()), so that is the only
 * test needed -- 4/2 is stored as 2/1.
 */
PHP_METHOD(OceanMoon_Math_Rational, isInt)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_long num, den;
	rational_read_parts(Z_OBJ_P(ZEND_THIS), &num, &den);

	RETURN_BOOL(den == 1);
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::sign(bool $zeroForZero = true): int
 *
 * Matches the PHP package's Rational::sign(), which delegates to Numbers::sign($this->numerator,
 * $zeroForZero): 1 for positive, -1 for negative, and for zero either 0 ($zeroForZero, the default)
 * or 1. The numerator carries the value's sign -- rational_init() normalises so the denominator is
 * always positive -- so it is the whole input the sign depends on.
 */
PHP_METHOD(OceanMoon_Math_Rational, sign)
{
	bool zero_for_zero = true;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(zero_for_zero)
	ZEND_PARSE_PARAMETERS_END();

	zend_long num, den;
	rational_read_parts(Z_OBJ_P(ZEND_THIS), &num, &den);

	if (num > 0) {
		RETURN_LONG(1);
	}
	if (num < 0) {
		RETURN_LONG(-1);
	}

	RETURN_LONG(zero_for_zero ? 0 : 1);
}
/* }}} */
