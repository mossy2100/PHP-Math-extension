/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 167bf46e06b28eda8f740690e12d7299036070ae */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OceanMoon_Math_Vector___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Vector_fromArray, 0, 1, OceanMoon\\Math\\Vector, 0)
	ZEND_ARG_TYPE_INFO(0, arr, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Vector_toArray, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Vector_toRowMatrix, 0, 0, OceanMoon\\Math\\Matrix, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Vector_toColumnMatrix arginfo_class_OceanMoon_Math_Vector_toRowMatrix

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Vector___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Vector_get, 0, 1, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Vector_set, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Vector_normalize, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Vector_equal, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, other, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Vector_approxEqual, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, other, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, relTol, IS_DOUBLE, 0, "1.0E-9")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, absTol, IS_DOUBLE, 0, "PHP_FLOAT_EPSILON")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Vector_neg, 0, 0, OceanMoon\\Math\\Vector, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Vector_reciprocal arginfo_class_OceanMoon_Math_Vector_neg

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Vector_add, 0, 1, OceanMoon\\Math\\Vector, 0)
	ZEND_ARG_OBJ_INFO(0, other, OceanMoon\\Math\\Vector, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Vector_sub arginfo_class_OceanMoon_Math_Vector_add

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Vector_mul, 0, 1, OceanMoon\\Math\\Vector, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, other, OceanMoon\\Math\\Matrix, MAY_BE_DOUBLE, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Vector_div, 0, 1, OceanMoon\\Math\\Vector, 0)
	ZEND_ARG_TYPE_INFO(0, scalar, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Vector_hadamardMul arginfo_class_OceanMoon_Math_Vector_add

#define arginfo_class_OceanMoon_Math_Vector_hadamardDiv arginfo_class_OceanMoon_Math_Vector_add

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Vector_dot, 0, 1, IS_DOUBLE, 0)
	ZEND_ARG_OBJ_INFO(0, other, OceanMoon\\Math\\Vector, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Vector_cross arginfo_class_OceanMoon_Math_Vector_add

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Vector_outer, 0, 1, OceanMoon\\Math\\Matrix, 0)
	ZEND_ARG_OBJ_INFO(0, other, OceanMoon\\Math\\Vector, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Vector_normalized arginfo_class_OceanMoon_Math_Vector_neg

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Vector_sum, 0, 0, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Vector_prod arginfo_class_OceanMoon_Math_Vector_sum

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Vector_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Vector_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Vector_offsetGet, 0, 1, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Vector_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Vector_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_METHOD(OceanMoon_Math_Vector, __construct);
ZEND_METHOD(OceanMoon_Math_Vector, fromArray);
ZEND_METHOD(OceanMoon_Math_Vector, toArray);
ZEND_METHOD(OceanMoon_Math_Vector, toRowMatrix);
ZEND_METHOD(OceanMoon_Math_Vector, toColumnMatrix);
ZEND_METHOD(OceanMoon_Math_Vector, __toString);
ZEND_METHOD(OceanMoon_Math_Vector, get);
ZEND_METHOD(OceanMoon_Math_Vector, set);
ZEND_METHOD(OceanMoon_Math_Vector, normalize);
ZEND_METHOD(OceanMoon_Math_Vector, equal);
ZEND_METHOD(OceanMoon_Math_Vector, approxEqual);
ZEND_METHOD(OceanMoon_Math_Vector, neg);
ZEND_METHOD(OceanMoon_Math_Vector, reciprocal);
ZEND_METHOD(OceanMoon_Math_Vector, add);
ZEND_METHOD(OceanMoon_Math_Vector, sub);
ZEND_METHOD(OceanMoon_Math_Vector, mul);
ZEND_METHOD(OceanMoon_Math_Vector, div);
ZEND_METHOD(OceanMoon_Math_Vector, hadamardMul);
ZEND_METHOD(OceanMoon_Math_Vector, hadamardDiv);
ZEND_METHOD(OceanMoon_Math_Vector, dot);
ZEND_METHOD(OceanMoon_Math_Vector, cross);
ZEND_METHOD(OceanMoon_Math_Vector, outer);
ZEND_METHOD(OceanMoon_Math_Vector, normalized);
ZEND_METHOD(OceanMoon_Math_Vector, sum);
ZEND_METHOD(OceanMoon_Math_Vector, prod);
ZEND_METHOD(OceanMoon_Math_Vector, count);
ZEND_METHOD(OceanMoon_Math_Vector, offsetExists);
ZEND_METHOD(OceanMoon_Math_Vector, offsetGet);
ZEND_METHOD(OceanMoon_Math_Vector, offsetSet);
ZEND_METHOD(OceanMoon_Math_Vector, offsetUnset);

static const zend_function_entry class_OceanMoon_Math_Vector_methods[] = {
	ZEND_ME(OceanMoon_Math_Vector, __construct, arginfo_class_OceanMoon_Math_Vector___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, fromArray, arginfo_class_OceanMoon_Math_Vector_fromArray, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(OceanMoon_Math_Vector, toArray, arginfo_class_OceanMoon_Math_Vector_toArray, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, toRowMatrix, arginfo_class_OceanMoon_Math_Vector_toRowMatrix, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, toColumnMatrix, arginfo_class_OceanMoon_Math_Vector_toColumnMatrix, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, __toString, arginfo_class_OceanMoon_Math_Vector___toString, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, get, arginfo_class_OceanMoon_Math_Vector_get, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, set, arginfo_class_OceanMoon_Math_Vector_set, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, normalize, arginfo_class_OceanMoon_Math_Vector_normalize, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, equal, arginfo_class_OceanMoon_Math_Vector_equal, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, approxEqual, arginfo_class_OceanMoon_Math_Vector_approxEqual, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, neg, arginfo_class_OceanMoon_Math_Vector_neg, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, reciprocal, arginfo_class_OceanMoon_Math_Vector_reciprocal, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, add, arginfo_class_OceanMoon_Math_Vector_add, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, sub, arginfo_class_OceanMoon_Math_Vector_sub, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, mul, arginfo_class_OceanMoon_Math_Vector_mul, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, div, arginfo_class_OceanMoon_Math_Vector_div, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, hadamardMul, arginfo_class_OceanMoon_Math_Vector_hadamardMul, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, hadamardDiv, arginfo_class_OceanMoon_Math_Vector_hadamardDiv, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, dot, arginfo_class_OceanMoon_Math_Vector_dot, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, cross, arginfo_class_OceanMoon_Math_Vector_cross, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, outer, arginfo_class_OceanMoon_Math_Vector_outer, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, normalized, arginfo_class_OceanMoon_Math_Vector_normalized, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, sum, arginfo_class_OceanMoon_Math_Vector_sum, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, prod, arginfo_class_OceanMoon_Math_Vector_prod, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, count, arginfo_class_OceanMoon_Math_Vector_count, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, offsetExists, arginfo_class_OceanMoon_Math_Vector_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, offsetGet, arginfo_class_OceanMoon_Math_Vector_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, offsetSet, arginfo_class_OceanMoon_Math_Vector_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Vector, offsetUnset, arginfo_class_OceanMoon_Math_Vector_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_OceanMoon_Math_Vector(zend_class_entry *class_entry_Stringable, zend_class_entry *class_entry_Countable, zend_class_entry *class_entry_ArrayAccess)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "OceanMoon\\Math", "Vector", class_OceanMoon_Math_Vector_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);
	zend_class_implements(class_entry, 3, class_entry_Stringable, class_entry_Countable, class_entry_ArrayAccess);

	zval property_data_default_value;
	ZVAL_EMPTY_ARRAY(&property_data_default_value);
	zend_string *property_data_name = zend_string_init("data", sizeof("data") - 1, 1);
	zend_declare_typed_property(class_entry, property_data_name, &property_data_default_value, ZEND_ACC_PRIVATE, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ARRAY));
	zend_string_release(property_data_name);

	zval property_size_default_value;
	ZVAL_LONG(&property_size_default_value, 0);
	zend_string *property_size_name = zend_string_init("size", sizeof("size") - 1, 1);
	zend_declare_typed_property(class_entry, property_size_name, &property_size_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_size_name);

	zval property_magnitude_default_value;
	ZVAL_NULL(&property_magnitude_default_value);
	zend_string *property_magnitude_name = zend_string_init("magnitude", sizeof("magnitude") - 1, 1);
	zend_declare_typed_property(class_entry, property_magnitude_name, &property_magnitude_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_DOUBLE|MAY_BE_NULL));
	zend_string_release(property_magnitude_name);

	return class_entry;
}
