/*
 * vector_modification.c
 *
 * Modification methods for OceanMoon\Math\Vector: set(), normalize(). Mirrors the "Modification
 * methods" region of the PHP package's Vector class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "vector_internal.h"
#include "vector_arginfo.h"
#include "exceptions.h"

/* {{{ OceanMoon\Math\Vector::set(int $index, float $value): void
 *
 * Matches the PHP package's Vector::set().
 */
PHP_METHOD(OceanMoon_Math_Vector, set)
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

/* {{{ OceanMoon\Math\Vector::normalize(): void
 *
 * Matches the PHP package's Vector::normalize(): `$unit = $this->normalized(); for (...)
 * $this->set($i, $unit->get($i));`. Computes the unit vector first via vector_calc_div() (the
 * same core normalized() uses -- see vector_linear_algebra.c), then copies its elements in --
 * if this vector has zero magnitude, vector_calc_div()'s ArithmeticException (the same "Cannot
 * divide by zero." div() itself throws) propagates before any element is written, leaving this
 * vector unmodified, matching the PHP package.
 */
PHP_METHOD(OceanMoon_Math_Vector, normalize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long size = vector_read_size(self);

	double magnitude;
	{
		zval rv;
		magnitude = zval_get_double(
			zend_read_property(vector_ce_Vector, self, "magnitude", sizeof("magnitude") - 1, 1, &rv));
	}

	zval unit;
	if (vector_calc_div(self, magnitude, &unit) == FAILURE) {
		RETURN_THROWS();
	}
	zend_object *unit_obj = Z_OBJ_P(&unit);

	for (zend_long i = 0; i < size; i++) {
		double element;
		vector_read_element(unit_obj, i, &element);
		if (vector_write_element(self, i, element) == FAILURE) {
			zval_ptr_dtor(&unit);
			RETURN_THROWS();
		}
	}

	zval_ptr_dtor(&unit);
}
/* }}} */
