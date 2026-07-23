/*
 * vector.c
 *
 * OceanMoon\Math\Vector: class registration, the constructor, factory
 * methods, and the shared internal helpers (vector_init/vector_create/
 * vector_read_element/vector_write_element) declared in vector_internal.h
 * and used by this file as well as the other vector_*.c implementation
 * files in this directory.
 *
 * Unlike Complex/Rational (which store their state as individual scalar properties), Vector's
 * $data is a genuine PHP array property (list<float>), matching the pure-PHP class exactly --
 * letting the Zend engine's own array/zval machinery handle allocation and refcounting, rather
 * than a custom object struct. Every read/write of an element goes through vector_read_element()/
 * vector_write_element() below, which is the one place bounds-checking, finite-value validation,
 * and the $magnitude cache invalidation (on write) all live.
 *
 * vector_minit() is called from the module's real PHP_MINIT_FUNCTION in ../math.c. Conversion
 * methods (toArray/__toString/toRowMatrix/toColumnMatrix) live in vector_conversion.c; the
 * computed $magnitude property lives in vector_properties.c; inspection/modification/comparison/
 * arithmetic/linear-algebra/aggregation/ArrayAccess methods live in their own vector_*.c files,
 * matching Vector.php's own region split.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"
#include "php_math.h"
#include "vector_internal.h"
#include "vector_arginfo.h"
#include "exceptions.h"
#include "floats.h"

/* The registered class entry for OceanMoon\Math\Vector. */
zend_class_entry *vector_ce_Vector;

/* {{{ vector_init
 *
 * Validate size and set up an already-allocated Vector object's $data (a zero-filled packed
 * array of $size doubles) and $size properties. Shared by __construct and vector_create() (used
 * by fromArray() and every arithmetic/linear-algebra method that builds a new Vector result).
 *
 * Returns FAILURE (with an exception already thrown) if size is negative.
 */
zend_result vector_init(zend_object *obj, zend_long size)
{
	if (size < 0) {
		zend_string *msg = strpprintf(0, "Cannot create Vector with negative size: " ZEND_LONG_FMT ".", size);
		zend_throw_exception(spl_ce_DomainException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	zval data;
	array_init_size(&data, (uint32_t) size);
	for (zend_long i = 0; i < size; i++) {
		zval zero;
		ZVAL_DOUBLE(&zero, 0.0);
		zend_hash_next_index_insert(Z_ARRVAL(data), &zero);
	}

	zend_update_property(vector_ce_Vector, obj, "data", sizeof("data") - 1, &data);
	zval_ptr_dtor(&data);

	zend_update_property_long(vector_ce_Vector, obj, "size", sizeof("size") - 1, size);

	return SUCCESS;
}
/* }}} */

/* {{{ vector_create
 *
 * Allocate a new Vector object into return_value and initialize it via vector_init().
 */
zend_result vector_create(zval *return_value, zend_long size)
{
	object_init_ex(return_value, vector_ce_Vector);
	return vector_init(Z_OBJ_P(return_value), size);
}
/* }}} */

/* {{{ vector_read_size
 *
 * Reads the $size property off a zend_object already known to be a Vector instance.
 */
zend_long vector_read_size(zend_object *obj)
{
	zval rv;
	zval *size = zend_read_property(vector_ce_Vector, obj, "size", sizeof("size") - 1, 1, &rv);
	return zval_get_long(size);
}
/* }}} */

/* {{{ vector_read_element
 *
 * Bounds-checked read of $data[$index] off a zend_object already known to be a Vector instance.
 * Used by get()/offsetGet() and, since it's cheap and correctness-by-construction is worth more
 * than a redundant check in the rare hot path, by every other method that reads elements too.
 *
 * Returns FAILURE (with OutOfRangeException already thrown) if the index is out of range.
 */
zend_result vector_read_element(zend_object *obj, zend_long index, double *out_value)
{
	zend_long size = vector_read_size(obj);

	if (index < 0 || index >= size) {
		zend_string *msg = strpprintf(
			0, "Invalid index: " ZEND_LONG_FMT ". Must be in the range 0-" ZEND_LONG_FMT ".", index, size - 1
		);
		zend_throw_exception(spl_ce_OutOfRangeException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	zval rv;
	zval *data = zend_read_property(vector_ce_Vector, obj, "data", sizeof("data") - 1, 1, &rv);
	zval *element = zend_hash_index_find(Z_ARRVAL_P(data), (zend_ulong) index);
	*out_value = Z_DVAL_P(element);

	return SUCCESS;
}
/* }}} */

/* {{{ vector_write_element
 *
 * Bounds-checked, finite-value-checked write of $data[$index] off a zend_object already known to
 * be a Vector instance, invalidating the cached $magnitude afterward (matching the PHP package's
 * set(): `$this->data[$index] = $value; $this->magnitude = null;`). Used by set()/offsetSet() and,
 * for the same correctness-by-construction reason as vector_read_element(), by every other method
 * that builds a new Vector result element-by-element -- this is also where a non-finite result
 * (e.g. a scalar multiplication overflowing to INF) gets caught.
 *
 * Returns FAILURE (with an exception already thrown) for an out-of-range index or a non-finite
 * value.
 */
zend_result vector_write_element(zend_object *obj, zend_long index, double value)
{
	zend_long size = vector_read_size(obj);

	if (index < 0 || index >= size) {
		zend_string *msg = strpprintf(
			0, "Invalid index: " ZEND_LONG_FMT ". Must be in the range 0-" ZEND_LONG_FMT ".", index, size - 1
		);
		zend_throw_exception(spl_ce_OutOfRangeException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	if (!zend_finite(value)) {
		zend_string *value_str = math_floats_to_str(value);
		zend_string *msg = strpprintf(0, "Cannot set element to non-finite value: %s.", ZSTR_VAL(value_str));
		zend_string_release(value_str);
		zend_throw_exception(spl_ce_DomainException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	zval rv;
	zval *data = zend_read_property(vector_ce_Vector, obj, "data", sizeof("data") - 1, 1, &rv);
	SEPARATE_ARRAY(data);
	zval new_value;
	ZVAL_DOUBLE(&new_value, value);
	zend_hash_index_update(Z_ARRVAL_P(data), (zend_ulong) index, &new_value);

	zend_update_property_null(vector_ce_Vector, obj, "magnitude", sizeof("magnitude") - 1);

	return SUCCESS;
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::__construct(int $size) */
PHP_METHOD(OceanMoon_Math_Vector, __construct)
{
	zend_long size;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(size)
	ZEND_PARSE_PARAMETERS_END();

	if (vector_init(Z_OBJ_P(ZEND_THIS), size) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ vector_minit
 *
 * Called from PHP_MINIT_FUNCTION(math) in ../math.c. Class entry, method table, and typed
 * properties are all generated from vector.stub.php into vector_arginfo.h; register them here.
 */
zend_result vector_minit(void)
{
	vector_ce_Vector = register_class_OceanMoon_Math_Vector(zend_ce_stringable, zend_ce_countable, zend_ce_arrayaccess);

	return vector_properties_minit();
}
/* }}} */
