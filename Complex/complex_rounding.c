/*
 * complex_rounding.c
 *
 * The round() method for OceanMoon\Math\Complex. Mirrors the "Rounding methods" region of the
 * PHP package's Complex class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/php_math.h"
#include "ext/standard/php_math_round_mode.h"
#include "complex_internal.h"
#include "complex_arginfo.h"

/* {{{ OceanMoon\Math\Complex::round(int $precision = 0, RoundingMode $mode = RoundingMode::HalfAwayFromZero): Complex
 *
 * Matches the PHP package's Complex::round(): rounds the real and imaginary parts independently,
 * each via PHP's own round(). Calls the same underlying engine primitives PHP's round() itself
 * uses -- _php_math_round() (ext/standard/php_math.h) for the rounding math, and
 * php_math_round_mode_from_enum() (ext/standard/php_math_round_mode.h) to translate the
 * RoundingMode enum case into the legacy PHP_ROUND_* int it expects -- rather than reimplementing
 * any of the eight rounding-mode behaviors here, guaranteeing byte-identical results with zero
 * risk of drift from PHP's own round().
 */
PHP_METHOD(OceanMoon_Math_Complex, round)
{
	zend_long precision = 0;
	zend_object *mode_obj = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(precision)
		Z_PARAM_OBJ_OF_CLASS_OR_NULL(mode_obj, rounding_mode_ce)
	ZEND_PARSE_PARAMETERS_END();

	int mode = mode_obj ? php_math_round_mode_from_enum(mode_obj) : PHP_ROUND_HALF_UP;

	double real, imag;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &real, &imag);

	double rounded_real = _php_math_round(real, (int) precision, mode);
	double rounded_imag = _php_math_round(imag, (int) precision, mode);

	if (complex_create(return_value, rounded_real, rounded_imag) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */
