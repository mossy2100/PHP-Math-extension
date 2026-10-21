/*
 * complex_inspection.c
 *
 * Inspection methods for Mossy2100\Math\Complex. Mirrors the "Inspection
 * methods" region of the PHP package's Complex class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_mossy2100_math.h"
#include "complex_internal.h"
#include "../../mossy2100_math_arginfo.h"

/* {{{ Mossy2100\Math\Complex::isReal(): bool
 *
 * Matches the PHP package's Complex::isReal(): true if the imaginary part is zero.
 */
PHP_METHOD(Mossy2100_Math_Complex, isReal)
{
	zend_object *self = Z_OBJ_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();

	double imag = zval_get_double(math_types_read_property(self, complex_offset_imaginary));

	RETURN_BOOL(imag == 0.0);
}
/* }}} */
