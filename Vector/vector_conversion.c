/*
 * vector_conversion.c
 *
 * Conversion methods for OceanMoon\Math\Vector: toArray(), toRowMatrix(), toColumnMatrix(),
 * __toString(). Mirrors the "Conversion methods" region of the PHP package's Vector class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "vector_internal.h"
#include "vector_arginfo.h"
#include "../Matrix/matrix_internal.h"
#include "Zend/zend_smart_str.h"

/* {{{ vector_to_row_matrix
 *
 * The shared core behind toRowMatrix(): builds a 1xN Matrix from this Vector's elements. Also used
 * by mul()'s Matrix branch (vector_arithmetic.c). Mirrors `Matrix::fromArray([$this->data])`.
 */
zend_result vector_to_row_matrix(zend_object *obj, zval *return_value)
{
	zend_long size = vector_read_size(obj);

	if (matrix_create(return_value, 1, size) == FAILURE) {
		return FAILURE;
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < size; i++) {
		double value;
		vector_read_element(obj, i, &value);
		if (matrix_write_element(result, 0, i, value) == FAILURE) {
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			return FAILURE;
		}
	}

	return SUCCESS;
}
/* }}} */

/* {{{ vector_to_column_matrix
 *
 * The shared core behind toColumnMatrix(): builds an Nx1 Matrix from this Vector's elements. Also
 * used by outer() (vector_linear_algebra.c). Mirrors Vector::toColumnMatrix()'s direct construction
 * (not via Matrix::fromArray(), which can't distinguish a 0x0 matrix from a 0x1 one).
 */
zend_result vector_to_column_matrix(zend_object *obj, zval *return_value)
{
	zend_long size = vector_read_size(obj);

	if (matrix_create(return_value, size, 1) == FAILURE) {
		return FAILURE;
	}
	zend_object *result = Z_OBJ_P(return_value);

	for (zend_long i = 0; i < size; i++) {
		double value;
		vector_read_element(obj, i, &value);
		if (matrix_write_element(result, i, 0, value) == FAILURE) {
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			return FAILURE;
		}
	}

	return SUCCESS;
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::toRowMatrix(): Matrix
 *
 * Matches the PHP package's Vector::toRowMatrix().
 */
PHP_METHOD(OceanMoon_Math_Vector, toRowMatrix)
{
	ZEND_PARSE_PARAMETERS_NONE();

	if (vector_to_row_matrix(Z_OBJ_P(ZEND_THIS), return_value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::toColumnMatrix(): Matrix
 *
 * Matches the PHP package's Vector::toColumnMatrix().
 */
PHP_METHOD(OceanMoon_Math_Vector, toColumnMatrix)
{
	ZEND_PARSE_PARAMETERS_NONE();

	if (vector_to_column_matrix(Z_OBJ_P(ZEND_THIS), return_value) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::toArray(): array
 *
 * Matches the PHP package's Vector::toArray(): a copy of the internal data. Arrays are
 * copy-on-write in PHP, so simply returning the property's own array (via RETURN_ZVAL with the
 * copy flag) is already safe -- the caller gets an independent array the moment either side
 * mutates it.
 */
PHP_METHOD(OceanMoon_Math_Vector, toArray)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zval rv;
	zval *data = zend_read_property(vector_ce_Vector, Z_OBJ_P(ZEND_THIS), "data", sizeof("data") - 1, 1, &rv);
	RETURN_ZVAL(data, 1, 0);
}
/* }}} */

/* {{{ OceanMoon\Math\Vector::__toString(): string
 *
 * Matches the PHP package's Vector::__toString(): '⟨' . implode(', ', $this->data) . '⟩'.
 */
PHP_METHOD(OceanMoon_Math_Vector, __toString)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long size = vector_read_size(self);

	smart_str buf = {0};
	smart_str_appendl(&buf, "\xE2\x9F\xA8", 3); /* U+27E8 MATHEMATICAL LEFT ANGLE BRACKET */

	for (zend_long i = 0; i < size; i++) {
		double element;
		vector_read_element(self, i, &element);

		if (i > 0) {
			smart_str_appends(&buf, ", ");
		}

		zval tmp;
		ZVAL_DOUBLE(&tmp, element);
		convert_to_string(&tmp);
		smart_str_append(&buf, Z_STR(tmp));
		zend_string_release(Z_STR(tmp));
	}

	smart_str_appendl(&buf, "\xE2\x9F\xA9", 3); /* U+27E9 MATHEMATICAL RIGHT ANGLE BRACKET */
	smart_str_0(&buf);
	RETURN_STR(buf.s);
}
/* }}} */
