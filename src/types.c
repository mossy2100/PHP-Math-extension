/*
 * types.c
 *
 * See types.h.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "ext/spl/spl_array.h"
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

zend_result math_types_array_iterator(zval *return_value, zval *array)
{
	object_init_ex(return_value, spl_ce_ArrayIterator);
	zend_call_known_instance_method_with_1_params(spl_ce_ArrayIterator->constructor, Z_OBJ_P(return_value), NULL, array);
	if (EG(exception)) {
		zval_ptr_dtor(return_value);
		ZVAL_UNDEF(return_value);
		return FAILURE;
	}
	return SUCCESS;
}

zend_object *math_types_clone_obj(zend_object *object)
{
	zend_object *new_obj = zend_objects_clone_obj(object);

	new_obj->handlers = object->handlers;
	return new_obj;
}

zend_result math_types_do_operation(math_types_operation calculate, uint8_t opcode, zval *result, zval *op1, zval *op2)
{
	zval value;

	ZVAL_UNDEF(&value);
	if (calculate(opcode, &value, op1, op2) == FAILURE) {
		return FAILURE;
	}

	/* For compound assignment, result still holds op1's old value, which is released here. Otherwise result is a
	 * fresh temporary, with nothing to release. */
	if (result == op1) {
		zval_ptr_dtor(result);
	}
	ZVAL_COPY_VALUE(result, &value);
	return SUCCESS;
}
