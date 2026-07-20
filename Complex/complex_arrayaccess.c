/*
 * complex_arrayaccess.c
 *
 * ArrayAccess methods for OceanMoon\Math\Complex: offsetExists(), offsetGet(), offsetSet(),
 * offsetUnset(). Mirrors the "ArrayAccess methods" region of the PHP package's Complex class --
 * $z[0]/$z[1] read the real/imaginary parts; the class is immutable, so offsetSet()/offsetUnset()
 * always throw.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "complex_internal.h"
#include "complex_arginfo.h"

/* {{{ OceanMoon\Math\Complex::offsetExists(mixed $offset): bool
 *
 * Matches the PHP package's Complex::offsetExists(): only the (strictly-typed) int 0 or 1 counts.
 */
PHP_METHOD(OceanMoon_Math_Complex, offsetExists)
{
	zval *offset;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset)
	ZEND_PARSE_PARAMETERS_END();

	bool exists = Z_TYPE_P(offset) == IS_LONG && (Z_LVAL_P(offset) == 0 || Z_LVAL_P(offset) == 1);
	RETURN_BOOL(exists);
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::offsetGet(mixed $offset): float
 *
 * Matches the PHP package's Complex::offsetGet(): $z[0] is real, $z[1] is imaginary.
 */
PHP_METHOD(OceanMoon_Math_Complex, offsetGet)
{
	zval *offset;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(offset) != IS_LONG) {
		zend_string *msg = strpprintf(
			0, "Invalid offset type: %s. Must be int.", zend_zval_type_name(offset));
		zend_throw_exception(spl_ce_InvalidArgumentException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		RETURN_THROWS();
	}

	zend_long lval = Z_LVAL_P(offset);
	if (lval != 0 && lval != 1) {
		zend_string *msg = strpprintf(0, "Invalid offset: " ZEND_LONG_FMT ". Must be 0 or 1.", lval);
		zend_throw_exception(spl_ce_OutOfRangeException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		RETURN_THROWS();
	}

	double real, imag;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &real, &imag);
	RETURN_DOUBLE(lval == 0 ? real : imag);
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::offsetSet(mixed $offset, mixed $value): void
 *
 * Matches the PHP package's Complex::offsetSet(): always throws, since Complex is immutable.
 */
PHP_METHOD(OceanMoon_Math_Complex, offsetSet)
{
	zval *offset, *value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(offset)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	zend_throw_exception(spl_ce_LogicException, "Complex values are immutable.", 0);
	RETURN_THROWS();
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::offsetUnset(mixed $offset): void
 *
 * Matches the PHP package's Complex::offsetUnset(): always throws, since Complex is immutable.
 */
PHP_METHOD(OceanMoon_Math_Complex, offsetUnset)
{
	zval *offset;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset)
	ZEND_PARSE_PARAMETERS_END();

	zend_throw_exception(spl_ce_LogicException, "Complex values are immutable.", 0);
	RETURN_THROWS();
}
/* }}} */
