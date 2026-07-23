/*
 * vector_inspection.c
 *
 * Inspection methods for OceanMoon\Math\Vector: get(). Mirrors the "Inspection methods" region of
 * the PHP package's Vector class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "vector_internal.h"
#include "vector_arginfo.h"

/* {{{ OceanMoon\Math\Vector::get(int $index): float
 *
 * Matches the PHP package's Vector::get().
 */
PHP_METHOD(OceanMoon_Math_Vector, get)
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
