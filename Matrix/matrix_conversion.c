/*
 * matrix_conversion.c
 *
 * Conversion methods for OceanMoon\Math\Matrix: toArray(), __toString(). Mirrors the "Conversion
 * methods" region of the PHP package's Matrix class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "matrix_internal.h"
#include "matrix_arginfo.h"
#include "Zend/zend_smart_str.h"

/* {{{ OceanMoon\Math\Matrix::toArray(): array
 *
 * Matches the PHP package's Matrix::toArray(): a rectangular list<list<float>> copy of the
 * matrix data.
 */
PHP_METHOD(OceanMoon_Math_Matrix, toArray)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);

	array_init_size(return_value, (uint32_t) row_count);
	for (zend_long i = 0; i < row_count; i++) {
		zval row_array;
		array_init_size(&row_array, (uint32_t) column_count);
		for (zend_long j = 0; j < column_count; j++) {
			double element;
			matrix_read_element(self, i, j, &element);
			zval v;
			ZVAL_DOUBLE(&v, element);
			zend_hash_next_index_insert(Z_ARRVAL(row_array), &v);
		}
		zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &row_array);
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Matrix::__toString(): string
 *
 * Matches the PHP package's Matrix::__toString(): box-drawing-character rendering, right-aligned
 * columns padded to the widest cell's width.
 */
PHP_METHOD(OceanMoon_Math_Matrix, __toString)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *self = Z_OBJ_P(ZEND_THIS);
	zend_long row_count = matrix_read_row_count(self);
	zend_long column_count = matrix_read_column_count(self);

	if (row_count == 0 || column_count == 0) {
		smart_str buf = {0};
		smart_str_appendl(&buf, "\xE2\x94\x8C \xE2\x94\x90\n\xE2\x94\x94 \xE2\x94\x98", 15);
		smart_str_0(&buf);
		RETURN_STR(buf.s);
	}

	/* Format every cell up front so column widths are calculated against the same strings that
	 * get rendered. */
	zend_string ***cells = safe_emalloc(sizeof(zend_string **), row_count, 0);
	size_t max_width = 0;
	for (zend_long i = 0; i < row_count; i++) {
		cells[i] = safe_emalloc(sizeof(zend_string *), column_count, 0);
		for (zend_long j = 0; j < column_count; j++) {
			double element;
			matrix_read_element(self, i, j, &element);

			zval tmp;
			ZVAL_DOUBLE(&tmp, element);
			convert_to_string(&tmp);
			cells[i][j] = Z_STR(tmp);

			if (ZSTR_LEN(cells[i][j]) > max_width) {
				max_width = ZSTR_LEN(cells[i][j]);
			}
		}
	}

	smart_str buf = {0};

	/* Top border. */
	size_t inner_width = (size_t) column_count * (max_width + 2);
	smart_str_appendl(&buf, "\xE2\x94\x8C", 3);
	for (size_t k = 0; k < inner_width; k++) {
		smart_str_appendc(&buf, ' ');
	}
	smart_str_appendl(&buf, "\xE2\x94\x90\n", 4);

	/* Data rows. */
	for (zend_long i = 0; i < row_count; i++) {
		smart_str_appendl(&buf, "\xE2\x94\x82 ", 4);
		for (zend_long j = 0; j < column_count; j++) {
			if (j > 0) {
				smart_str_appends(&buf, "  ");
			}
			size_t pad = max_width - ZSTR_LEN(cells[i][j]);
			for (size_t k = 0; k < pad; k++) {
				smart_str_appendc(&buf, ' ');
			}
			smart_str_append(&buf, cells[i][j]);
		}
		smart_str_appendl(&buf, " \xE2\x94\x82\n", 5);
	}

	/* Bottom border. */
	smart_str_appendl(&buf, "\xE2\x94\x94", 3);
	for (size_t k = 0; k < inner_width; k++) {
		smart_str_appendc(&buf, ' ');
	}
	smart_str_appendl(&buf, "\xE2\x94\x98", 3);

	for (zend_long i = 0; i < row_count; i++) {
		for (zend_long j = 0; j < column_count; j++) {
			zend_string_release(cells[i][j]);
		}
		efree(cells[i]);
	}
	efree(cells);

	smart_str_0(&buf);
	RETURN_STR(buf.s);
}
/* }}} */
