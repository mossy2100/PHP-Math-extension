/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 4b2ac5b88650cf337463feadd9f16a8a9a750702 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OceanMoon_Math_Complex___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, real, IS_DOUBLE, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, imag, IS_DOUBLE, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Complex___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Complex_toArray, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Complex_toObject, 0, 0, stdClass, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Complex_isReal, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Complex_equal, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, other, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Complex_approxEqual, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, other, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, relTol, IS_DOUBLE, 0, "1.0E-9")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, absTol, IS_DOUBLE, 0, "PHP_FLOAT_EPSILON")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Complex_fromArray, 0, 1, OceanMoon\\Math\\Complex, 0)
	ZEND_ARG_TYPE_INFO(0, arr, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Complex_fromObject, 0, 1, OceanMoon\\Math\\Complex, 0)
	ZEND_ARG_TYPE_INFO(0, obj, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Complex_fromString, 0, 1, OceanMoon\\Math\\Complex, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Complex_fromPolar, 0, 2, OceanMoon\\Math\\Complex, 0)
	ZEND_ARG_TYPE_INFO(0, mag, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, phase, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Complex_toComplex, 0, 1, OceanMoon\\Math\\Complex, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_METHOD(OceanMoon_Math_Complex, __construct);
ZEND_METHOD(OceanMoon_Math_Complex, __toString);
ZEND_METHOD(OceanMoon_Math_Complex, toArray);
ZEND_METHOD(OceanMoon_Math_Complex, toObject);
ZEND_METHOD(OceanMoon_Math_Complex, isReal);
ZEND_METHOD(OceanMoon_Math_Complex, equal);
ZEND_METHOD(OceanMoon_Math_Complex, approxEqual);
ZEND_METHOD(OceanMoon_Math_Complex, fromArray);
ZEND_METHOD(OceanMoon_Math_Complex, fromObject);
ZEND_METHOD(OceanMoon_Math_Complex, fromString);
ZEND_METHOD(OceanMoon_Math_Complex, fromPolar);
ZEND_METHOD(OceanMoon_Math_Complex, toComplex);

static const zend_function_entry class_OceanMoon_Math_Complex_methods[] = {
	ZEND_ME(OceanMoon_Math_Complex, __construct, arginfo_class_OceanMoon_Math_Complex___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, __toString, arginfo_class_OceanMoon_Math_Complex___toString, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, toArray, arginfo_class_OceanMoon_Math_Complex_toArray, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, toObject, arginfo_class_OceanMoon_Math_Complex_toObject, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, isReal, arginfo_class_OceanMoon_Math_Complex_isReal, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, equal, arginfo_class_OceanMoon_Math_Complex_equal, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, approxEqual, arginfo_class_OceanMoon_Math_Complex_approxEqual, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, fromArray, arginfo_class_OceanMoon_Math_Complex_fromArray, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(OceanMoon_Math_Complex, fromObject, arginfo_class_OceanMoon_Math_Complex_fromObject, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(OceanMoon_Math_Complex, fromString, arginfo_class_OceanMoon_Math_Complex_fromString, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(OceanMoon_Math_Complex, fromPolar, arginfo_class_OceanMoon_Math_Complex_fromPolar, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(OceanMoon_Math_Complex, toComplex, arginfo_class_OceanMoon_Math_Complex_toComplex, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_OceanMoon_Math_Complex(zend_class_entry *class_entry_Stringable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "OceanMoon\\Math", "Complex", class_OceanMoon_Math_Complex_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);
	zend_class_implements(class_entry, 1, class_entry_Stringable);

	zval property_real_default_value;
	ZVAL_DOUBLE(&property_real_default_value, 0.0);
	zend_string *property_real_name = zend_string_init("real", sizeof("real") - 1, 1);
	zend_declare_typed_property(class_entry, property_real_name, &property_real_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_DOUBLE));
	zend_string_release(property_real_name);

	zval property_imaginary_default_value;
	ZVAL_DOUBLE(&property_imaginary_default_value, 0.0);
	zend_string *property_imaginary_name = zend_string_init("imaginary", sizeof("imaginary") - 1, 1);
	zend_declare_typed_property(class_entry, property_imaginary_name, &property_imaginary_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_DOUBLE));
	zend_string_release(property_imaginary_name);

	return class_entry;
}
