/*
 * vector_inspection.c
 *
 * Inspection methods for Mossy2100\Math\Vector: get(). Mirrors the "Inspection methods" region of
 * the PHP package's Vector class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "vector_internal.h"
#include "../../mossy2100_math_arginfo.h"

/* {{{ Mossy2100\Math\Vector::get(int $index): float
 *
 * Matches the PHP package's Vector::get().
 */
PHP_METHOD(Mossy2100_Math_Vector, get)
{
	zend_long index;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(index)
	ZEND_PARSE_PARAMETERS_END();

	double value;
	if (vector_read_element(Z_OBJ_P(ZEND_THIS), index, &value) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_DOUBLE(value);
}
/* }}} */
