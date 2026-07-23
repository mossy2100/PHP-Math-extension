/*
 * vector_aggregation.c
 *
 * Aggregation methods for OceanMoon\Math\Vector: sum(), prod(), count(). Mirrors the
 * "Aggregation methods" region of the PHP package's Vector class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "vector_internal.h"
#include "vector_arginfo.h"

/* {{{ OceanMoon\Math\Vector::sum(): float
 *
 * Matches the PHP package's Vector::sum(): 0.0 for an empty vector.
 */
PHP_METHOD(OceanMoon_Math_Vector, sum)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long size = vector_read_size(self);

	double sum = 0.0;
	for (zend_long i = 0; i < size; i++) {
		double element;
		vector_read_element(self, i, &element);
		sum += element;
	}

	RETURN_DOUBLE(sum);
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::prod(): float
 *
 * Matches the PHP package's Vector::prod(): 1.0 for an empty vector.
 */
PHP_METHOD(OceanMoon_Math_Vector, prod)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long size = vector_read_size(self);

	double prod = 1.0;
	for (zend_long i = 0; i < size; i++) {
		double element;
		vector_read_element(self, i, &element);
		prod *= element;
	}

	RETURN_DOUBLE(prod);
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::count(): int
 *
 * Matches the PHP package's Vector::count() (Countable): returns $size.
 */
PHP_METHOD(OceanMoon_Math_Vector, count)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_LONG(vector_read_size(Z_OBJ_P(ZEND_THIS)));
}
/* }}} */
