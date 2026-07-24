/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: df5f5333daf0c1359acddb056fe4ac1e4ae2870c */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OceanMoon_Math_Matrix___construct, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, rowCount, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, columnCount, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_fromArray, 0, 1, OceanMoon\\Math\\Matrix, 0)
	ZEND_ARG_TYPE_INFO(0, arr, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_identity, 0, 1, OceanMoon\\Math\\Matrix, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_toArray, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Matrix___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_isSquare, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, size, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_get, 0, 2, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, row, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, col, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_getRow, 0, 1, OceanMoon\\Math\\Vector, 0)
	ZEND_ARG_TYPE_INFO(0, row, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_getColumn, 0, 1, OceanMoon\\Math\\Vector, 0)
	ZEND_ARG_TYPE_INFO(0, col, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_copy, 0, 4, OceanMoon\\Math\\Matrix, 0)
	ZEND_ARG_TYPE_INFO(0, row, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, col, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, rowCount, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, colCount, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_set, 0, 3, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, row, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, col, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_setRow, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, row, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, vec, OceanMoon\\Math\\Vector, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_setColumn, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, col, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, vec, OceanMoon\\Math\\Vector, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_paste, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, other, OceanMoon\\Math\\Matrix, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, row, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, col, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_equal, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, other, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_approxEqual, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, other, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, relTol, IS_DOUBLE, 0, "1.0E-9")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, absTol, IS_DOUBLE, 0, "PHP_FLOAT_EPSILON")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_resize, 0, 2, OceanMoon\\Math\\Matrix, 0)
	ZEND_ARG_TYPE_INFO(0, rowCount, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, columnCount, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_neg, 0, 0, OceanMoon\\Math\\Matrix, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Matrix_reciprocal arginfo_class_OceanMoon_Math_Matrix_neg

#define arginfo_class_OceanMoon_Math_Matrix_inv arginfo_class_OceanMoon_Math_Matrix_neg

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_add, 0, 1, OceanMoon\\Math\\Matrix, 0)
	ZEND_ARG_OBJ_INFO(0, other, OceanMoon\\Math\\Matrix, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Matrix_sub arginfo_class_OceanMoon_Math_Matrix_add

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_mul, 0, 1, OceanMoon\\Math\\Matrix, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, other, OceanMoon\\Math\\Matrix, MAY_BE_DOUBLE, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_div, 0, 1, OceanMoon\\Math\\Matrix, 0)
	ZEND_ARG_TYPE_INFO(0, scalar, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Matrix_hadamardMul arginfo_class_OceanMoon_Math_Matrix_add

#define arginfo_class_OceanMoon_Math_Matrix_hadamardDiv arginfo_class_OceanMoon_Math_Matrix_add

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_pow, 0, 1, OceanMoon\\Math\\Matrix, 0)
	ZEND_ARG_TYPE_INFO(0, exponent, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Matrix_sqr arginfo_class_OceanMoon_Math_Matrix_neg

#define arginfo_class_OceanMoon_Math_Matrix_t arginfo_class_OceanMoon_Math_Matrix_neg

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_det, 0, 0, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Matrix_trace arginfo_class_OceanMoon_Math_Matrix_det

#define arginfo_class_OceanMoon_Math_Matrix_norm arginfo_class_OceanMoon_Math_Matrix_det

#define arginfo_class_OceanMoon_Math_Matrix_p1Norm arginfo_class_OceanMoon_Math_Matrix_det

#define arginfo_class_OceanMoon_Math_Matrix_pInfNorm arginfo_class_OceanMoon_Math_Matrix_det

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_offsetGet, 0, 1, OceanMoon\\Math\\Vector, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_METHOD(OceanMoon_Math_Matrix, __construct);
ZEND_METHOD(OceanMoon_Math_Matrix, fromArray);
ZEND_METHOD(OceanMoon_Math_Matrix, identity);
ZEND_METHOD(OceanMoon_Math_Matrix, toArray);
ZEND_METHOD(OceanMoon_Math_Matrix, __toString);
ZEND_METHOD(OceanMoon_Math_Matrix, isSquare);
ZEND_METHOD(OceanMoon_Math_Matrix, get);
ZEND_METHOD(OceanMoon_Math_Matrix, getRow);
ZEND_METHOD(OceanMoon_Math_Matrix, getColumn);
ZEND_METHOD(OceanMoon_Math_Matrix, copy);
ZEND_METHOD(OceanMoon_Math_Matrix, set);
ZEND_METHOD(OceanMoon_Math_Matrix, setRow);
ZEND_METHOD(OceanMoon_Math_Matrix, setColumn);
ZEND_METHOD(OceanMoon_Math_Matrix, paste);
ZEND_METHOD(OceanMoon_Math_Matrix, equal);
ZEND_METHOD(OceanMoon_Math_Matrix, approxEqual);
ZEND_METHOD(OceanMoon_Math_Matrix, resize);
ZEND_METHOD(OceanMoon_Math_Matrix, neg);
ZEND_METHOD(OceanMoon_Math_Matrix, reciprocal);
ZEND_METHOD(OceanMoon_Math_Matrix, inv);
ZEND_METHOD(OceanMoon_Math_Matrix, add);
ZEND_METHOD(OceanMoon_Math_Matrix, sub);
ZEND_METHOD(OceanMoon_Math_Matrix, mul);
ZEND_METHOD(OceanMoon_Math_Matrix, div);
ZEND_METHOD(OceanMoon_Math_Matrix, hadamardMul);
ZEND_METHOD(OceanMoon_Math_Matrix, hadamardDiv);
ZEND_METHOD(OceanMoon_Math_Matrix, pow);
ZEND_METHOD(OceanMoon_Math_Matrix, sqr);
ZEND_METHOD(OceanMoon_Math_Matrix, t);
ZEND_METHOD(OceanMoon_Math_Matrix, det);
ZEND_METHOD(OceanMoon_Math_Matrix, trace);
ZEND_METHOD(OceanMoon_Math_Matrix, norm);
ZEND_METHOD(OceanMoon_Math_Matrix, p1Norm);
ZEND_METHOD(OceanMoon_Math_Matrix, pInfNorm);
ZEND_METHOD(OceanMoon_Math_Matrix, count);
ZEND_METHOD(OceanMoon_Math_Matrix, offsetExists);
ZEND_METHOD(OceanMoon_Math_Matrix, offsetGet);
ZEND_METHOD(OceanMoon_Math_Matrix, offsetSet);
ZEND_METHOD(OceanMoon_Math_Matrix, offsetUnset);

static const zend_function_entry class_OceanMoon_Math_Matrix_methods[] = {
	ZEND_ME(OceanMoon_Math_Matrix, __construct, arginfo_class_OceanMoon_Math_Matrix___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, fromArray, arginfo_class_OceanMoon_Math_Matrix_fromArray, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(OceanMoon_Math_Matrix, identity, arginfo_class_OceanMoon_Math_Matrix_identity, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(OceanMoon_Math_Matrix, toArray, arginfo_class_OceanMoon_Math_Matrix_toArray, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, __toString, arginfo_class_OceanMoon_Math_Matrix___toString, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, isSquare, arginfo_class_OceanMoon_Math_Matrix_isSquare, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, get, arginfo_class_OceanMoon_Math_Matrix_get, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, getRow, arginfo_class_OceanMoon_Math_Matrix_getRow, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, getColumn, arginfo_class_OceanMoon_Math_Matrix_getColumn, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, copy, arginfo_class_OceanMoon_Math_Matrix_copy, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, set, arginfo_class_OceanMoon_Math_Matrix_set, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, setRow, arginfo_class_OceanMoon_Math_Matrix_setRow, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, setColumn, arginfo_class_OceanMoon_Math_Matrix_setColumn, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, paste, arginfo_class_OceanMoon_Math_Matrix_paste, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, equal, arginfo_class_OceanMoon_Math_Matrix_equal, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, approxEqual, arginfo_class_OceanMoon_Math_Matrix_approxEqual, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, resize, arginfo_class_OceanMoon_Math_Matrix_resize, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, neg, arginfo_class_OceanMoon_Math_Matrix_neg, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, reciprocal, arginfo_class_OceanMoon_Math_Matrix_reciprocal, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, inv, arginfo_class_OceanMoon_Math_Matrix_inv, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, add, arginfo_class_OceanMoon_Math_Matrix_add, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, sub, arginfo_class_OceanMoon_Math_Matrix_sub, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, mul, arginfo_class_OceanMoon_Math_Matrix_mul, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, div, arginfo_class_OceanMoon_Math_Matrix_div, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, hadamardMul, arginfo_class_OceanMoon_Math_Matrix_hadamardMul, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, hadamardDiv, arginfo_class_OceanMoon_Math_Matrix_hadamardDiv, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, pow, arginfo_class_OceanMoon_Math_Matrix_pow, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, sqr, arginfo_class_OceanMoon_Math_Matrix_sqr, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, t, arginfo_class_OceanMoon_Math_Matrix_t, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, det, arginfo_class_OceanMoon_Math_Matrix_det, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, trace, arginfo_class_OceanMoon_Math_Matrix_trace, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, norm, arginfo_class_OceanMoon_Math_Matrix_norm, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, p1Norm, arginfo_class_OceanMoon_Math_Matrix_p1Norm, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, pInfNorm, arginfo_class_OceanMoon_Math_Matrix_pInfNorm, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, count, arginfo_class_OceanMoon_Math_Matrix_count, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, offsetExists, arginfo_class_OceanMoon_Math_Matrix_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, offsetGet, arginfo_class_OceanMoon_Math_Matrix_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, offsetSet, arginfo_class_OceanMoon_Math_Matrix_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Matrix, offsetUnset, arginfo_class_OceanMoon_Math_Matrix_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_OceanMoon_Math_Matrix(zend_class_entry *class_entry_Stringable, zend_class_entry *class_entry_Countable, zend_class_entry *class_entry_ArrayAccess)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "OceanMoon\\Math", "Matrix", class_OceanMoon_Math_Matrix_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);
	zend_class_implements(class_entry, 3, class_entry_Stringable, class_entry_Countable, class_entry_ArrayAccess);

	zval property_data_default_value;
	ZVAL_EMPTY_ARRAY(&property_data_default_value);
	zend_string *property_data_name = zend_string_init("data", sizeof("data") - 1, 1);
	zend_declare_typed_property(class_entry, property_data_name, &property_data_default_value, ZEND_ACC_PRIVATE, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ARRAY));
	zend_string_release(property_data_name);

	zval property_rowCount_default_value;
	ZVAL_LONG(&property_rowCount_default_value, 0);
	zend_string *property_rowCount_name = zend_string_init("rowCount", sizeof("rowCount") - 1, 1);
	zend_declare_typed_property(class_entry, property_rowCount_name, &property_rowCount_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_rowCount_name);

	zval property_columnCount_default_value;
	ZVAL_LONG(&property_columnCount_default_value, 0);
	zend_string *property_columnCount_name = zend_string_init("columnCount", sizeof("columnCount") - 1, 1);
	zend_declare_typed_property(class_entry, property_columnCount_name, &property_columnCount_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_columnCount_name);

	return class_entry;
}
