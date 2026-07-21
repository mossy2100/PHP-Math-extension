/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 643d92bdb12eb1b08482552602d103efd586bf93 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OceanMoon_Math_Rational___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, num, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, den, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Rational_fromFloat, 0, 1, OceanMoon\\Math\\Rational, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Rational_fromString, 0, 1, OceanMoon\\Math\\Rational, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Rational_toFloat, 0, 0, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Rational_toMixedNumber, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Rational___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Rational_compare, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, other, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Rational_equal, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, other, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Rational_lessThan arginfo_class_OceanMoon_Math_Rational_equal

#define arginfo_class_OceanMoon_Math_Rational_lessThanOrEqual arginfo_class_OceanMoon_Math_Rational_equal

#define arginfo_class_OceanMoon_Math_Rational_greaterThan arginfo_class_OceanMoon_Math_Rational_equal

#define arginfo_class_OceanMoon_Math_Rational_greaterThanOrEqual arginfo_class_OceanMoon_Math_Rational_equal

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Rational_approxEqual, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, other, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, relTol, IS_DOUBLE, 0, "1.0E-9")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, absTol, IS_DOUBLE, 0, "PHP_FLOAT_EPSILON")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Rational_approxCompare, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, other, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, relTol, IS_DOUBLE, 0, "1.0E-9")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, absTol, IS_DOUBLE, 0, "PHP_FLOAT_EPSILON")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Rational_abs, 0, 0, OceanMoon\\Math\\Rational, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Rational_neg arginfo_class_OceanMoon_Math_Rational_abs

#define arginfo_class_OceanMoon_Math_Rational_inv arginfo_class_OceanMoon_Math_Rational_abs

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Rational_add, 0, 1, OceanMoon\\Math\\Rational, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, other, OceanMoon\\Math\\Rational, MAY_BE_LONG, NULL)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Rational_sub arginfo_class_OceanMoon_Math_Rational_add

#define arginfo_class_OceanMoon_Math_Rational_mul arginfo_class_OceanMoon_Math_Rational_add

#define arginfo_class_OceanMoon_Math_Rational_div arginfo_class_OceanMoon_Math_Rational_add

ZEND_METHOD(OceanMoon_Math_Rational, __construct);
ZEND_METHOD(OceanMoon_Math_Rational, fromFloat);
ZEND_METHOD(OceanMoon_Math_Rational, fromString);
ZEND_METHOD(OceanMoon_Math_Rational, toFloat);
ZEND_METHOD(OceanMoon_Math_Rational, toMixedNumber);
ZEND_METHOD(OceanMoon_Math_Rational, __toString);
ZEND_METHOD(OceanMoon_Math_Rational, compare);
ZEND_METHOD(OceanMoon_Math_Rational, equal);
ZEND_METHOD(OceanMoon_Math_Rational, lessThan);
ZEND_METHOD(OceanMoon_Math_Rational, lessThanOrEqual);
ZEND_METHOD(OceanMoon_Math_Rational, greaterThan);
ZEND_METHOD(OceanMoon_Math_Rational, greaterThanOrEqual);
ZEND_METHOD(OceanMoon_Math_Rational, approxEqual);
ZEND_METHOD(OceanMoon_Math_Rational, approxCompare);
ZEND_METHOD(OceanMoon_Math_Rational, abs);
ZEND_METHOD(OceanMoon_Math_Rational, neg);
ZEND_METHOD(OceanMoon_Math_Rational, inv);
ZEND_METHOD(OceanMoon_Math_Rational, add);
ZEND_METHOD(OceanMoon_Math_Rational, sub);
ZEND_METHOD(OceanMoon_Math_Rational, mul);
ZEND_METHOD(OceanMoon_Math_Rational, div);

static const zend_function_entry class_OceanMoon_Math_Rational_methods[] = {
	ZEND_ME(OceanMoon_Math_Rational, __construct, arginfo_class_OceanMoon_Math_Rational___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Rational, fromFloat, arginfo_class_OceanMoon_Math_Rational_fromFloat, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(OceanMoon_Math_Rational, fromString, arginfo_class_OceanMoon_Math_Rational_fromString, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(OceanMoon_Math_Rational, toFloat, arginfo_class_OceanMoon_Math_Rational_toFloat, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Rational, toMixedNumber, arginfo_class_OceanMoon_Math_Rational_toMixedNumber, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Rational, __toString, arginfo_class_OceanMoon_Math_Rational___toString, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Rational, compare, arginfo_class_OceanMoon_Math_Rational_compare, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Rational, equal, arginfo_class_OceanMoon_Math_Rational_equal, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Rational, lessThan, arginfo_class_OceanMoon_Math_Rational_lessThan, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Rational, lessThanOrEqual, arginfo_class_OceanMoon_Math_Rational_lessThanOrEqual, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Rational, greaterThan, arginfo_class_OceanMoon_Math_Rational_greaterThan, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Rational, greaterThanOrEqual, arginfo_class_OceanMoon_Math_Rational_greaterThanOrEqual, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Rational, approxEqual, arginfo_class_OceanMoon_Math_Rational_approxEqual, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Rational, approxCompare, arginfo_class_OceanMoon_Math_Rational_approxCompare, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Rational, abs, arginfo_class_OceanMoon_Math_Rational_abs, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Rational, neg, arginfo_class_OceanMoon_Math_Rational_neg, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Rational, inv, arginfo_class_OceanMoon_Math_Rational_inv, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Rational, add, arginfo_class_OceanMoon_Math_Rational_add, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Rational, sub, arginfo_class_OceanMoon_Math_Rational_sub, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Rational, mul, arginfo_class_OceanMoon_Math_Rational_mul, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Rational, div, arginfo_class_OceanMoon_Math_Rational_div, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_OceanMoon_Math_Rational(zend_class_entry *class_entry_Stringable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "OceanMoon\\Math", "Rational", class_OceanMoon_Math_Rational_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);
	zend_class_implements(class_entry, 1, class_entry_Stringable);

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
