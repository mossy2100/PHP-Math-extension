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

void math_types_mark_read_only(zend_class_entry *ce, const char *const *names, size_t count)
{
	for (size_t i = 0; i < count; i++) {
		zend_property_info *info = zend_hash_str_find_ptr(&ce->properties_info, names[i], strlen(names[i]));

		if (info != NULL) {
			info->flags |= ZEND_ACC_PRIVATE_SET;
		}
	}
}

uint32_t math_types_property_offset(zend_class_entry *ce, const char *name)
{
	zend_property_info *info = zend_hash_str_find_ptr(&ce->properties_info, name, strlen(name));

	/* Every name passed here is declared in oceanmoon_math.stub.php, so a miss is a bug in the
	 * extension itself, not something a user can trigger. */
	ZEND_ASSERT(info != NULL && !(info->flags & ZEND_ACC_STATIC));

	return info->offset;
}
