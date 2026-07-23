/*
 * vector_arrayaccess.c
 *
 * ArrayAccess methods for OceanMoon\Math\Vector: offsetExists(), offsetGet(), offsetSet(),
 * offsetUnset(). Mirrors the "ArrayAccess methods" region of the PHP package's Vector class --
 * unlike Complex (immutable), Vector is mutable, so offsetSet() actually writes; only
 * offsetUnset() always throws, since Vector has a fixed size.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "vector_internal.h"
#include "vector_arginfo.h"
#include "types.h"

/* {{{ OceanMoon\Math\Vector::offsetExists(mixed $offset): bool
 *
 * Matches the PHP package's Vector::offsetExists().
 */
PHP_METHOD(OceanMoon_Math_Vector, offsetExists)
{
	zval *offset;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(offset) != IS_LONG) {
		RETURN_FALSE;
	}

	zend_long index = Z_LVAL_P(offset);
	zend_long size = vector_read_size(Z_OBJ_P(ZEND_THIS));
	RETURN_BOOL(index >= 0 && index < size);
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::offsetGet(mixed $offset): float
 *
 * Matches the PHP package's Vector::offsetGet().
 */
PHP_METHOD(OceanMoon_Math_Vector, offsetGet)
{
	zval *offset;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(offset) != IS_LONG) {
		zend_string *msg = strpprintf(
			0, "Invalid index type: %s. Must be int.", math_types_debug_type_name(offset));
		zend_throw_exception(spl_ce_InvalidArgumentException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		RETURN_THROWS();
	}

	double value;
	if (vector_read_element(Z_OBJ_P(ZEND_THIS), Z_LVAL_P(offset), &value) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_DOUBLE(value);
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::offsetSet(mixed $offset, mixed $value): void
 *
 * Matches the PHP package's Vector::offsetSet().
 */
PHP_METHOD(OceanMoon_Math_Vector, offsetSet)
{
	zval *offset, *value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(offset)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(offset) != IS_LONG) {
		zend_string *msg = strpprintf(
			0, "Invalid index type: %s. Must be int.", math_types_debug_type_name(offset));
		zend_throw_exception(spl_ce_InvalidArgumentException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		RETURN_THROWS();
	}

	if (Z_TYPE_P(value) != IS_LONG && Z_TYPE_P(value) != IS_DOUBLE) {
		zend_string *msg = strpprintf(
			0, "Invalid element type: %s. Must be int or float.", math_types_debug_type_name(value));
		zend_throw_exception(spl_ce_InvalidArgumentException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		RETURN_THROWS();
	}

	double dval = Z_TYPE_P(value) == IS_LONG ? (double) Z_LVAL_P(value) : Z_DVAL_P(value);
	if (vector_write_element(Z_OBJ_P(ZEND_THIS), Z_LVAL_P(offset), dval) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::offsetUnset(mixed $offset): void
 *
 * Matches the PHP package's Vector::offsetUnset(): always throws, since Vector has a fixed size.
 */
PHP_METHOD(OceanMoon_Math_Vector, offsetUnset)
{
	zval *offset;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset)
	ZEND_PARSE_PARAMETERS_END();

	zend_throw_exception(spl_ce_LogicException, "Cannot unset Vector elements.", 0);
	RETURN_THROWS();
}
/* }}} */
