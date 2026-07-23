/*
 * matrix.c
 *
 * OceanMoon\Math\Matrix: class registration, the constructor, and the shared internal helpers
 * (matrix_init/matrix_create/matrix_read_row_count/matrix_read_column_count/matrix_read_row/
 * matrix_read_element/matrix_write_element) declared in matrix_internal.h and used by this file as
 * well as the other matrix_*.c implementation files in this directory.
 *
 * Matrix's $data is a genuine PHP array property, one Vector object per row (list<Vector>),
 * matching the pure-PHP class exactly -- see Matrix.php's own $data property doc comment for why
 * row Vectors are mutated in place rather than replaced (offsetGet() hands out a live reference).
 *
 * This is also where the one genuinely new C pattern in this plan lives: a custom `clone_obj`
 * object handler. PHP's default object clone (zend_objects_clone_obj) copies the $data array
 * itself (cheap, COW) but not the Vector *objects* referenced inside it -- a cloned Matrix would
 * share row Vectors with the original, so mutating a row of one (via set()/setRow()/offsetSet())
 * would silently mutate the other too. matrix_clone_obj() below does the default shallow clone
 * first, then deep-clones each row Vector, exactly mirroring Matrix::__clone()'s
 * `array_map(fn ($v) => clone $v, $this->data)` in the pure-PHP package.
 *
 * matrix_minit() is called from the module's real PHP_MINIT_FUNCTION in ../math.c.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string.h>

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"
#include "Zend/zend_objects.h"
#include "php_math.h"
#include "matrix_internal.h"
#include "matrix_arginfo.h"
#include "exceptions.h"
#include "floats.h"

/* The registered class entry for OceanMoon\Math\Matrix. */
zend_class_entry *matrix_ce_Matrix;

/* The custom object handlers for Matrix, installed by matrix_create_object(). A copy of the
 * standard handlers with `clone_obj` overridden so cloning deep-clones row Vectors, matching the
 * PHP package's __clone(). */
static zend_object_handlers matrix_object_handlers;

/* {{{ matrix_init
 *
 * Validate dimensions and set up an already-allocated Matrix object's $data (one fresh Vector per
 * row), $rowCount, and $columnCount properties. Shared by __construct and matrix_create() (used by
 * fromArray()/identity() and every arithmetic/linear-algebra method that builds a new Matrix
 * result).
 *
 * Returns FAILURE (with an exception already thrown) if either dimension is negative.
 */
zend_result matrix_init(zend_object *obj, zend_long row_count, zend_long column_count)
{
	if (row_count < 0) {
		zend_string *msg = strpprintf(
			0, "Cannot create Matrix with " ZEND_LONG_FMT " rows. Must not be negative.", row_count
		);
		zend_throw_exception(spl_ce_DomainException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}
	if (column_count < 0) {
		zend_string *msg = strpprintf(
			0, "Cannot create Matrix with " ZEND_LONG_FMT " columns. Must not be negative.", column_count
		);
		zend_throw_exception(spl_ce_DomainException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	zval data;
	array_init_size(&data, (uint32_t) row_count);
	for (zend_long i = 0; i < row_count; i++) {
		zval row;
		if (vector_create(&row, column_count) == FAILURE) {
			zval_ptr_dtor(&data);
			return FAILURE;
		}
		zend_hash_next_index_insert(Z_ARRVAL(data), &row);
	}

	zend_update_property(matrix_ce_Matrix, obj, "data", sizeof("data") - 1, &data);
	zval_ptr_dtor(&data);

	zend_update_property_long(matrix_ce_Matrix, obj, "rowCount", sizeof("rowCount") - 1, row_count);
	zend_update_property_long(matrix_ce_Matrix, obj, "columnCount", sizeof("columnCount") - 1, column_count);

	return SUCCESS;
}
/* }}} */

/* {{{ matrix_create
 *
 * Allocate a new Matrix object into return_value and initialize it via matrix_init().
 */
zend_result matrix_create(zval *return_value, zend_long row_count, zend_long column_count)
{
	object_init_ex(return_value, matrix_ce_Matrix);
	return matrix_init(Z_OBJ_P(return_value), row_count, column_count);
}
/* }}} */

/* {{{ matrix_read_row_count */
zend_long matrix_read_row_count(zend_object *obj)
{
	zval rv;
	zval *row_count = zend_read_property(matrix_ce_Matrix, obj, "rowCount", sizeof("rowCount") - 1, 1, &rv);
	return zval_get_long(row_count);
}
/* }}} */

/* {{{ matrix_read_column_count */
zend_long matrix_read_column_count(zend_object *obj)
{
	zval rv;
	zval *column_count = zend_read_property(
		matrix_ce_Matrix, obj, "columnCount", sizeof("columnCount") - 1, 1, &rv);
	return zval_get_long(column_count);
}
/* }}} */

/* {{{ matrix_read_row
 *
 * Returns the zend_object* for the live row Vector at $data[$row], without bounds checking --
 * callers that accept caller-supplied row indexes must bounds-check first (matching
 * vector_read_element()'s convention).
 */
zend_object *matrix_read_row(zend_object *obj, zend_long row)
{
	zval rv;
	zval *data = zend_read_property(matrix_ce_Matrix, obj, "data", sizeof("data") - 1, 1, &rv);
	zval *row_zval = zend_hash_index_find(Z_ARRVAL_P(data), (zend_ulong) row);
	return Z_OBJ_P(row_zval);
}
/* }}} */

/* {{{ matrix_read_element
 *
 * Bounds-checked read of $data[$row][$col] off a zend_object already known to be a Matrix
 * instance. Mirrors Matrix::get().
 *
 * Returns FAILURE (with OutOfRangeException already thrown) if either index is out of range.
 */
zend_result matrix_read_element(zend_object *obj, zend_long row, zend_long col, double *out_value)
{
	zend_long row_count = matrix_read_row_count(obj);
	if (row < 0 || row >= row_count) {
		zend_string *msg = strpprintf(
			0, "Invalid row index: " ZEND_LONG_FMT ". Must be in the range 0-" ZEND_LONG_FMT ".",
			row, row_count - 1
		);
		zend_throw_exception(spl_ce_OutOfRangeException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	zend_long column_count = matrix_read_column_count(obj);
	if (col < 0 || col >= column_count) {
		zend_string *msg = strpprintf(
			0, "Invalid column index: " ZEND_LONG_FMT ". Must be in the range 0-" ZEND_LONG_FMT ".",
			col, column_count - 1
		);
		zend_throw_exception(spl_ce_OutOfRangeException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	return vector_read_element(matrix_read_row(obj, row), col, out_value);
}
/* }}} */

/* {{{ matrix_write_element
 *
 * Bounds-checked, finite-value-checked write of $data[$row][$col] off a zend_object already known
 * to be a Matrix instance. Mirrors Matrix::set(). Delegates the actual write (and its own
 * finite-value check) to vector_write_element() on the row's Vector.
 *
 * Returns FAILURE (with an exception already thrown) for an out-of-range index or a non-finite
 * value.
 */
zend_result matrix_write_element(zend_object *obj, zend_long row, zend_long col, double value)
{
	zend_long row_count = matrix_read_row_count(obj);
	if (row < 0 || row >= row_count) {
		zend_string *msg = strpprintf(
			0, "Invalid row index: " ZEND_LONG_FMT ". Must be in the range 0-" ZEND_LONG_FMT ".",
			row, row_count - 1
		);
		zend_throw_exception(spl_ce_OutOfRangeException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	zend_long column_count = matrix_read_column_count(obj);
	if (col < 0 || col >= column_count) {
		zend_string *msg = strpprintf(
			0, "Invalid column index: " ZEND_LONG_FMT ". Must be in the range 0-" ZEND_LONG_FMT ".",
			col, column_count - 1
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

	return vector_write_element(matrix_read_row(obj, row), col, value);
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::__construct(int $rowCount, int $columnCount) */
PHP_METHOD(OceanMoon_Math_Matrix, __construct)
{
	zend_long row_count, column_count;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(row_count)
		Z_PARAM_LONG(column_count)
	ZEND_PARSE_PARAMETERS_END();

	if (matrix_init(Z_OBJ_P(ZEND_THIS), row_count, column_count) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ matrix_clone_obj
 *
 * Custom `clone_obj` handler: performs the standard shallow clone (copies scalar properties and
 * the $data array itself), then deep-clones each row Vector so the clone doesn't share row
 * identity with the original. See the file-header comment for why this is necessary.
 */
static zend_object *matrix_clone_obj(zend_object *object)
{
	zend_object *new_obj = zend_objects_clone_obj(object);

	zend_long row_count = matrix_read_row_count(new_obj);

	zval rv;
	zval *data = zend_read_property(matrix_ce_Matrix, new_obj, "data", sizeof("data") - 1, 1, &rv);
	SEPARATE_ARRAY(data);
	HashTable *ht = Z_ARRVAL_P(data);

	for (zend_long i = 0; i < row_count; i++) {
		zval *row = zend_hash_index_find(ht, (zend_ulong) i);
		zend_object *row_obj = Z_OBJ_P(row);
		zend_object *cloned_row = row_obj->handlers->clone_obj(row_obj);
		zval cloned_zval;
		ZVAL_OBJ(&cloned_zval, cloned_row);
		zend_hash_index_update(ht, (zend_ulong) i, &cloned_zval);
	}

	return new_obj;
}
/* }}} */

/* {{{ matrix_create_object
 *
 * Installs matrix_object_handlers on every new Matrix instance in place of the class entry's
 * default (std_object_handlers). Registered as matrix_ce_Matrix->create_object in matrix_minit().
 */
static zend_object *matrix_create_object(zend_class_entry *ce)
{
	zend_object *obj = zend_objects_new(ce);
	object_properties_init(obj, ce);
	obj->handlers = &matrix_object_handlers;
	return obj;
}
/* }}} */

/* {{{ matrix_minit
 *
 * Called from PHP_MINIT_FUNCTION(math) in ../math.c. Class entry, method table, and typed
 * properties are all generated from matrix.stub.php into matrix_arginfo.h; register them here.
 */
zend_result matrix_minit(void)
{
	matrix_ce_Matrix = register_class_OceanMoon_Math_Matrix(zend_ce_stringable, zend_ce_countable, zend_ce_arrayaccess);

	memcpy(&matrix_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	matrix_object_handlers.clone_obj = matrix_clone_obj;
	matrix_object_handlers.do_operation = matrix_do_operation;
	matrix_ce_Matrix->create_object = matrix_create_object;

	return SUCCESS;
}
/* }}} */
