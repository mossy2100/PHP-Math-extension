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
 * $data is a genuine PHP array property (list<float>), matching the pure-PHP class exactly -
 * letting the Zend engine's own array/zval machinery handle allocation and refcounting, rather
 * than a custom object struct. Every read/write of an element goes through vector_read_element()/
 * vector_write_element() below, which is the one place bounds-checking and finite-value validation
 * live.
 *
 * vector_minit() is called from the module's real PHP_MINIT_FUNCTION in ../oceanmoon_math.c. Conversion
 * methods (toArray/__toString/toRowMatrix/toColumnMatrix) live in vector_conversion.c; the
 * magnitude() method lives in vector_linear_algebra.c; inspection/modification/comparison/
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
#include "php_oceanmoon_math.h"
#include "vector_internal.h"
#include "../../oceanmoon_math_arginfo.h"
#include "exceptions.h"
#include "floats.h"

/* The registered class entry for OceanMoon\Math\Vector. */
zend_class_entry *vector_ce_Vector;
uint32_t vector_offset_count;
uint32_t vector_offset_data;

/* {{{ vector_init
 *
 * Validate count and set up an already-allocated Vector object's $data (a zero-filled packed
 * array of $count doubles) and $count properties. Shared by __construct and vector_create() (used
 * by fromArray() and every arithmetic/linear-algebra method that builds a new Vector result).
 *
 * Returns FAILURE (with an exception already thrown) if count is negative.
 */
zend_result vector_init(zend_object *obj, zend_long count)
{
	if (count < 0) {
		zend_string *msg = strpprintf(0, "Cannot create Vector with negative count: " ZEND_LONG_FMT ".", count);
		zend_throw_exception(spl_ce_DomainException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	zval data;
	array_init_size(&data, (uint32_t) count);
	for (zend_long i = 0; i < count; i++) {
		zval zero;
		ZVAL_DOUBLE(&zero, 0.0);
		zend_hash_next_index_insert(Z_ARRVAL(data), &zero);
	}

	/* Moves data into the slot (no extra reference to release). */
	math_types_write_property(obj, vector_offset_data, &data);

	zval count_zval;
	ZVAL_LONG(&count_zval, count);
	math_types_write_property(obj, vector_offset_count, &count_zval);

	return SUCCESS;
}
/* }}} */

/* {{{ vector_create
 *
 * Allocate a new Vector object into return_value and initialize it via vector_init().
 */
zend_result vector_create(zval *return_value, zend_long count)
{
	object_init_ex(return_value, vector_ce_Vector);
	return vector_init(Z_OBJ_P(return_value), count);
}
/* }}} */

/* {{{ vector_read_count
 *
 * Reads the $count property off a zend_object already known to be a Vector instance.
 */
zend_long vector_read_count(zend_object *obj)
{
	return zval_get_long(math_types_read_property(obj, vector_offset_count));
}
/* }}} */

/* {{{ vector_throw_index_error
 *
 * Throws OutOfRangeException for an index outside 0 to count - 1, matching the package's Vector::indexException():
 * "Invalid index: 3. Must be in the range 0-2.", or "Invalid index: 0. The Vector is empty." if count is 0.
 */
void vector_throw_index_error(zend_long index, zend_long count)
{
	zend_string *msg = count == 0
		? strpprintf(0, "Invalid index: " ZEND_LONG_FMT ". The Vector is empty.", index)
		: strpprintf(
			0, "Invalid index: " ZEND_LONG_FMT ". Must be in the range 0-" ZEND_LONG_FMT ".", index, count - 1
		);
	zend_throw_exception(spl_ce_OutOfRangeException, ZSTR_VAL(msg), 0);
	zend_string_release(msg);
}
/* }}} */

/* {{{ vector_read_element
 *
 * Bounds-checked read of $data[$index] off a zend_object already known to be a Vector instance.
 * Used by get()/offsetGet() and, since it's cheap and correctness-by-construction is worth more
 * than a redundant check in the rare hot path, by every other method that reads elements too.
 *
 * Returns FAILURE (with OutOfRangeException already thrown) if the index is out of range, or with Error thrown if
 * $data doesn't hold a float at the index. That can only happen if $data or $count were changed through Reflection,
 * or by unserialize(), but it would otherwise read a NULL pointer, or a pointer's bits as a double.
 */
zend_result vector_read_element(zend_object *obj, zend_long index, double *out_value)
{
	zend_long count = vector_read_count(obj);

	if (index < 0 || index >= count) {
		vector_throw_index_error(index, count);
		return FAILURE;
	}

	zval *data = math_types_read_property(obj, vector_offset_data);
	zval *element = zend_hash_index_find(Z_ARRVAL_P(data), (zend_ulong) index);
	if (UNEXPECTED(element == NULL || Z_TYPE_P(element) != IS_DOUBLE)) {
		zend_throw_error(NULL, "Vector data is invalid.");
		return FAILURE;
	}
	*out_value = Z_DVAL_P(element);

	return SUCCESS;
}
/* }}} */

/* {{{ vector_write_element
 *
 * Bounds-checked, finite-value-checked write of $data[$index] off a zend_object already known to
 * be a Vector instance. Used by set()/offsetSet() and, for the same correctness-by-construction
 * reason as vector_read_element(), by every other method that builds a new Vector result
 * element-by-element - this is also where a non-finite result (e.g. a scalar multiplication
 * overflowing to INF) gets caught, and where a negative zero is stored as a positive zero.
 *
 * Returns FAILURE (with an exception already thrown) for an out-of-range index or a non-finite
 * value.
 */
zend_result vector_write_element(zend_object *obj, zend_long index, double value)
{
	zend_long count = vector_read_count(obj);

	if (index < 0 || index >= count) {
		vector_throw_index_error(index, count);
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

	zval *data = math_types_read_property(obj, vector_offset_data);
	SEPARATE_ARRAY(data);
	zval new_value;
	ZVAL_DOUBLE(&new_value, math_floats_normalize_zero(value));
	zend_hash_index_update(Z_ARRVAL_P(data), (zend_ulong) index, &new_value);

	return SUCCESS;
}
/* }}} */

/* {{{ vector_write_result
 *
 * Like vector_write_element(), for a calculated result. Matches the PHP package's Vector::checkResult(): with
 * finite operands, a non-finite result means it (or an intermediate value) overflowed, so it throws
 * OverflowException "Cannot <operation>. The result would overflow." rather than set()'s DomainException.
 */
zend_result vector_write_result(zend_object *obj, zend_long index, double value, const char *operation)
{
	if (!zend_finite(value)) {
		math_throw_overflow(operation);
		return FAILURE;
	}
	return vector_write_element(obj, index, value);
}
/* }}} */

/* {{{ vector_read_all
 *
 * Reads every element into a new buffer of vector_read_count() doubles, which the caller frees with efree(). Faster
 * than calling vector_read_element() for each element, since it reads $count and $data only once. Returns NULL, with
 * Error thrown, if $data is invalid (see vector_read_element()).
 */
double *vector_read_all(zend_object *obj)
{
	zend_long count = vector_read_count(obj);
	HashTable *ht = Z_ARRVAL_P(math_types_read_property(obj, vector_offset_data));

	/* Check the size first, so a $count changed through Reflection can't make the allocation fail. */
	if (UNEXPECTED(count < 0 || zend_hash_num_elements(ht) != (uint32_t) count)) {
		zend_throw_error(NULL, "Vector data is invalid.");
		return NULL;
	}

	double *values = safe_emalloc((size_t) (count > 0 ? count : 1), sizeof(double), 0);
	if (vector_read_into(obj, values) == FAILURE) {
		efree(values);
		return NULL;
	}
	return values;
}
/* }}} */

/* {{{ vector_read_into
 *
 * Reads every element into values, which has room for vector_read_count() doubles. Like vector_read_all(), for a
 * caller that reads several Vectors into one buffer, e.g. a Matrix's rows. Returns FAILURE, with Error thrown, if
 * $data is invalid (see vector_read_element()).
 */
zend_result vector_read_into(zend_object *obj, double *values)
{
	zend_long count = vector_read_count(obj);
	HashTable *ht = Z_ARRVAL_P(math_types_read_property(obj, vector_offset_data));

	for (zend_long i = 0; i < count; i++) {
		zval *element = zend_hash_index_find(ht, (zend_ulong) i);
		if (UNEXPECTED(element == NULL || Z_TYPE_P(element) != IS_DOUBLE)) {
			zend_throw_error(NULL, "Vector data is invalid.");
			return FAILURE;
		}
		values[i] = Z_DVAL_P(element);
	}
	return SUCCESS;
}
/* }}} */

/* {{{ vector_create_from
 *
 * Allocates a new Vector into return_value holding count elements copied from values, with negative zeros stored as
 * positive zeros. Faster than vector_create() then vector_write_result() for each element, since it builds $data in
 * one pass. Every value must be finite, so the caller checks results as it calculates them. count must not be
 * negative.
 */
void vector_create_from(zval *return_value, zend_long count, const double *values)
{
	object_init_ex(return_value, vector_ce_Vector);
	zend_object *obj = Z_OBJ_P(return_value);

	zval data;
	array_init_size(&data, (uint32_t) count);
	zend_hash_real_init_packed(Z_ARRVAL(data));
	ZEND_HASH_FILL_PACKED(Z_ARRVAL(data)) {
		for (zend_long i = 0; i < count; i++) {
			ZEND_ASSERT(zend_finite(values[i]));
			ZEND_HASH_FILL_SET_DOUBLE(math_floats_normalize_zero(values[i]));
			ZEND_HASH_FILL_NEXT();
		}
	} ZEND_HASH_FILL_END();

	/* Moves data into the slot (no extra reference to release). */
	math_types_write_property(obj, vector_offset_data, &data);

	zval count_zval;
	ZVAL_LONG(&count_zval, count);
	math_types_write_property(obj, vector_offset_count, &count_zval);
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::__construct(int $count) */
PHP_METHOD(OceanMoon_Math_Vector, __construct)
{
	zend_long count;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(count)
	ZEND_PARSE_PARAMETERS_END();

	if (vector_init(Z_OBJ_P(ZEND_THIS), count) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ vector_minit
 *
 * Called from PHP_MINIT_FUNCTION(oceanmoon_math) in ../oceanmoon_math.c. Class entry, method table, and typed
 * properties are all generated from oceanmoon_math.stub.php into oceanmoon_math_arginfo.h; register them here.
 */
zend_result vector_minit(void)
{
	vector_ce_Vector = register_class_OceanMoon_Math_Vector(
		zend_ce_stringable, zend_ce_countable, zend_ce_arrayaccess, zend_ce_aggregate
	);
	vector_offset_count = math_types_property_offset(vector_ce_Vector, "count");
	vector_offset_data = math_types_property_offset(vector_ce_Vector, "data");

	return vector_properties_minit();
}
/* }}} */
