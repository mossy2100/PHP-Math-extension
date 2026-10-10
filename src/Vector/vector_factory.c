/*
 * vector_factory.c
 *
 * Factory methods for OceanMoon\Math\Vector: fromArray(). Mirrors the PHP package's
 * Vector::fromArray(): validates the argument is a list of finite int|float values, then builds a
 * Vector from it.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "vector_internal.h"
#include "../../oceanmoon_math_arginfo.h"
#include "floats.h"

/* {{{ OceanMoon\Math\Vector::fromArray(array $arr): Vector */
PHP_METHOD(OceanMoon_Math_Vector, fromArray)
{
	zend_array *arr;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(arr)
	ZEND_PARSE_PARAMETERS_END();

	zend_ulong count = zend_hash_num_elements(arr);

	/* Handle empty input. */
	if (count == 0) {
		if (vector_create(return_value, 0) == FAILURE) {
			RETURN_THROWS();
		}
		return;
	}

	/* Check for list. */
	if (!zend_array_is_list(arr)) {
		zend_throw_exception(spl_ce_DomainException, "Cannot create Vector from array. Must be a list.", 0);
		RETURN_THROWS();
	}

	if (vector_create(return_value, (zend_long) count) == FAILURE) {
		RETURN_THROWS();
	}
	zend_object *obj = Z_OBJ_P(return_value);

	zend_long index = 0;
	zval *value;
	ZEND_HASH_FOREACH_VAL(arr, value)
	{
		/* Check the value is a number. */
		if (Z_TYPE_P(value) != IS_LONG && Z_TYPE_P(value) != IS_DOUBLE) {
			zend_string *msg = strpprintf(
				0, "Cannot create Vector from array. Element at index " ZEND_LONG_FMT " must be an int or float.", index
			);
			zend_throw_exception(spl_ce_InvalidArgumentException, ZSTR_VAL(msg), 0);
			zend_string_release(msg);
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			RETURN_THROWS();
		}

		/* Check the value is finite. */
		if (Z_TYPE_P(value) == IS_DOUBLE && !zend_finite(Z_DVAL_P(value))) {
			zend_string *value_str = math_floats_to_str(Z_DVAL_P(value));
			zend_string *msg = strpprintf(
				0, "Cannot create Vector from array. Element at index " ZEND_LONG_FMT " (%s) must be finite.",
				index, ZSTR_VAL(value_str)
			);
			zend_string_release(value_str);
			zend_throw_exception(spl_ce_DomainException, ZSTR_VAL(msg), 0);
			zend_string_release(msg);
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			RETURN_THROWS();
		}

		double element = Z_TYPE_P(value) == IS_LONG ? (double) Z_LVAL_P(value) : Z_DVAL_P(value);
		if (vector_write_element(obj, index, element) == FAILURE) {
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			RETURN_THROWS();
		}

		index++;
	}
	ZEND_HASH_FOREACH_END();
}
/* }}} */
