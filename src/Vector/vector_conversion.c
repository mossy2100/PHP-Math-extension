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
#include "../../oceanmoon_math_arginfo.h"
#include "../Matrix/matrix_internal.h"
#include "Zend/zend_smart_str.h"

/* {{{ vector_to_row_matrix
 *
 * The shared core behind toRowMatrix(): builds a 1xN Matrix from this Vector's elements. Also used
 * by mul()'s Matrix branch (vector_arithmetic.c). Mirrors `Matrix::fromArray([$this->data])`.
 */
zend_result vector_to_row_matrix(zend_object *obj, zval *return_value)
{
	double *values = vector_read_all(obj);
	if (values == NULL) {
		return FAILURE;
	}
	matrix_create_from(return_value, 1, vector_read_count(obj), values);
	efree(values);
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
	double *values = vector_read_all(obj);
	if (values == NULL) {
		return FAILURE;
	}
	matrix_create_from(return_value, vector_read_count(obj), 1, values);
	efree(values);
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
 * copy flag) is already safe - the caller gets an independent array the moment either side
 * mutates it.
 */
PHP_METHOD(OceanMoon_Math_Vector, toArray)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zval *data = math_types_read_property(Z_OBJ_P(ZEND_THIS), vector_offset_data);
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
	zend_long count = vector_read_count(self);
	double *values = vector_read_all(self);
	if (values == NULL) {
		RETURN_THROWS();
	}

	smart_str buf = {0};
	smart_str_appendl(&buf, "\xE2\x9F\xA8", 3); /* U+27E8 MATHEMATICAL LEFT ANGLE BRACKET */

	for (zend_long i = 0; i < count; i++) {
		double element = values[i];

		if (i > 0) {
			smart_str_appends(&buf, ", ");
		}

		zval tmp;
		ZVAL_DOUBLE(&tmp, element);
		convert_to_string(&tmp);
		smart_str_append(&buf, Z_STR(tmp));
		zend_string_release(Z_STR(tmp));
	}

	efree(values);
	smart_str_appendl(&buf, "\xE2\x9F\xA9", 3); /* U+27E9 MATHEMATICAL RIGHT ANGLE BRACKET */
	smart_str_0(&buf);
	RETURN_STR(buf.s);
}
/* }}} */
