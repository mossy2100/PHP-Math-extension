/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: d2adb791236fa3fc1e8afd6e7b98d9850636e481 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OceanMoon_Math_Complex___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, real, IS_DOUBLE, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, imag, IS_DOUBLE, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Complex___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_METHOD(OceanMoon_Math_Complex, __construct);
ZEND_METHOD(OceanMoon_Math_Complex, __toString);

static const zend_function_entry class_OceanMoon_Math_Complex_methods[] = {
	ZEND_ME(OceanMoon_Math_Complex, __construct, arginfo_class_OceanMoon_Math_Complex___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, __toString, arginfo_class_OceanMoon_Math_Complex___toString, ZEND_ACC_PUBLIC)
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
