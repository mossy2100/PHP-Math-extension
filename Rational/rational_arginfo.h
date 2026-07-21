/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 6749525d1cbd088e6e98ce698d938d40991cc0ba */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OceanMoon_Math_Rational___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, num, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, den, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

ZEND_METHOD(OceanMoon_Math_Rational, __construct);

static const zend_function_entry class_OceanMoon_Math_Rational_methods[] = {
	ZEND_ME(OceanMoon_Math_Rational, __construct, arginfo_class_OceanMoon_Math_Rational___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_OceanMoon_Math_Rational(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "OceanMoon\\Math", "Rational", class_OceanMoon_Math_Rational_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);

	zval property_numerator_default_value;
	ZVAL_LONG(&property_numerator_default_value, 0);
	zend_string *property_numerator_name = zend_string_init("numerator", sizeof("numerator") - 1, 1);
	zend_declare_typed_property(class_entry, property_numerator_name, &property_numerator_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_numerator_name);

	zval property_denominator_default_value;
	ZVAL_LONG(&property_denominator_default_value, 1);
	zend_string *property_denominator_name = zend_string_init("denominator", sizeof("denominator") - 1, 1);
	zend_declare_typed_property(class_entry, property_denominator_name, &property_denominator_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_denominator_name);

	return class_entry;
}
