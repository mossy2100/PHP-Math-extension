/*
 * vector_modification.c
 *
 * Modification methods for Mossy2100\Math\Vector: set(). Mirrors the "Modification methods"
 * region of the PHP package's Vector class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "vector_internal.h"
#include "../../mossy2100_math_arginfo.h"
#include "exceptions.h"

/* {{{ Mossy2100\Math\Vector::set(int $index, float $value): void
 *
 * Matches the PHP package's Vector::set().
 */
PHP_METHOD(Mossy2100_Math_Vector, set)
{
	zend_long index;
	double value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(index)
		Z_PARAM_DOUBLE(value)
	ZEND_PARSE_PARAMETERS_END();

	if (vector_write_element(Z_OBJ_P(ZEND_THIS), index, value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */
