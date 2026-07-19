/*
 * types.c
 *
 * See types.h.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "types.h"

const char *math_types_debug_type_name(zval *value)
{
	switch (Z_TYPE_P(value)) {
		case IS_NULL: return "null";
		case IS_TRUE:
		case IS_FALSE: return "bool";
		case IS_LONG: return "int";
		case IS_DOUBLE: return "float";
		case IS_STRING: return "string";
		case IS_ARRAY: return "array";
		case IS_OBJECT: return ZSTR_VAL(Z_OBJCE_P(value)->name);
		default: return "unknown type";
	}
}
