/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 382be597b4445130abc685fa8c7243957cd18d16 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OceanMoon_Math_Complex___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, real, IS_DOUBLE, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, imag, IS_DOUBLE, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Complex_fromString, 0, 1, OceanMoon\\Math\\Complex, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Complex_fromPolar, 0, 2, OceanMoon\\Math\\Complex, 0)
	ZEND_ARG_TYPE_INFO(0, mag, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, phase, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Complex___toString, 0, 0, IS_STRING, 0)
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

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Complex_neg, 0, 0, OceanMoon\\Math\\Complex, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Complex_inv arginfo_class_OceanMoon_Math_Complex_neg

#define arginfo_class_OceanMoon_Math_Complex_conj arginfo_class_OceanMoon_Math_Complex_neg

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Complex_add, 0, 1, OceanMoon\\Math\\Complex, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, other, OceanMoon\\Math\\Complex, MAY_BE_DOUBLE, NULL)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Complex_sub arginfo_class_OceanMoon_Math_Complex_add

#define arginfo_class_OceanMoon_Math_Complex_mul arginfo_class_OceanMoon_Math_Complex_add

#define arginfo_class_OceanMoon_Math_Complex_div arginfo_class_OceanMoon_Math_Complex_add

#define arginfo_class_OceanMoon_Math_Complex_pow arginfo_class_OceanMoon_Math_Complex_add

#define arginfo_class_OceanMoon_Math_Complex_sqr arginfo_class_OceanMoon_Math_Complex_neg

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Complex_roots, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, degree, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Complex_sqrt arginfo_class_OceanMoon_Math_Complex_neg

#define arginfo_class_OceanMoon_Math_Complex_exp arginfo_class_OceanMoon_Math_Complex_neg

#define arginfo_class_OceanMoon_Math_Complex_ln arginfo_class_OceanMoon_Math_Complex_neg

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Complex_log, 0, 1, OceanMoon\\Math\\Complex, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, base, OceanMoon\\Math\\Complex, MAY_BE_DOUBLE, NULL)
ZEND_END_ARG_INFO()

ZEND_METHOD(OceanMoon_Math_Complex, __construct);
ZEND_METHOD(OceanMoon_Math_Complex, fromString);
ZEND_METHOD(OceanMoon_Math_Complex, fromPolar);
ZEND_METHOD(OceanMoon_Math_Complex, __toString);
ZEND_METHOD(OceanMoon_Math_Complex, isReal);
ZEND_METHOD(OceanMoon_Math_Complex, equal);
ZEND_METHOD(OceanMoon_Math_Complex, approxEqual);
ZEND_METHOD(OceanMoon_Math_Complex, neg);
ZEND_METHOD(OceanMoon_Math_Complex, inv);
ZEND_METHOD(OceanMoon_Math_Complex, conj);
ZEND_METHOD(OceanMoon_Math_Complex, add);
ZEND_METHOD(OceanMoon_Math_Complex, sub);
ZEND_METHOD(OceanMoon_Math_Complex, mul);
ZEND_METHOD(OceanMoon_Math_Complex, div);
ZEND_METHOD(OceanMoon_Math_Complex, pow);
ZEND_METHOD(OceanMoon_Math_Complex, sqr);
ZEND_METHOD(OceanMoon_Math_Complex, roots);
ZEND_METHOD(OceanMoon_Math_Complex, sqrt);
ZEND_METHOD(OceanMoon_Math_Complex, exp);
ZEND_METHOD(OceanMoon_Math_Complex, ln);
ZEND_METHOD(OceanMoon_Math_Complex, log);

static const zend_function_entry class_OceanMoon_Math_Complex_methods[] = {
	ZEND_ME(OceanMoon_Math_Complex, __construct, arginfo_class_OceanMoon_Math_Complex___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, fromString, arginfo_class_OceanMoon_Math_Complex_fromString, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(OceanMoon_Math_Complex, fromPolar, arginfo_class_OceanMoon_Math_Complex_fromPolar, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(OceanMoon_Math_Complex, __toString, arginfo_class_OceanMoon_Math_Complex___toString, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, isReal, arginfo_class_OceanMoon_Math_Complex_isReal, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, equal, arginfo_class_OceanMoon_Math_Complex_equal, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, approxEqual, arginfo_class_OceanMoon_Math_Complex_approxEqual, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, neg, arginfo_class_OceanMoon_Math_Complex_neg, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, inv, arginfo_class_OceanMoon_Math_Complex_inv, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, conj, arginfo_class_OceanMoon_Math_Complex_conj, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, add, arginfo_class_OceanMoon_Math_Complex_add, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, sub, arginfo_class_OceanMoon_Math_Complex_sub, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, mul, arginfo_class_OceanMoon_Math_Complex_mul, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, div, arginfo_class_OceanMoon_Math_Complex_div, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, pow, arginfo_class_OceanMoon_Math_Complex_pow, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, sqr, arginfo_class_OceanMoon_Math_Complex_sqr, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, roots, arginfo_class_OceanMoon_Math_Complex_roots, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, sqrt, arginfo_class_OceanMoon_Math_Complex_sqrt, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, exp, arginfo_class_OceanMoon_Math_Complex_exp, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, ln, arginfo_class_OceanMoon_Math_Complex_ln, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, log, arginfo_class_OceanMoon_Math_Complex_log, ZEND_ACC_PUBLIC)
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

	zval property_magnitude_default_value;
	ZVAL_NULL(&property_magnitude_default_value);
	zend_string *property_magnitude_name = zend_string_init("magnitude", sizeof("magnitude") - 1, 1);
	zend_declare_typed_property(class_entry, property_magnitude_name, &property_magnitude_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_DOUBLE|MAY_BE_NULL));
	zend_string_release(property_magnitude_name);

	zval property_phase_default_value;
	ZVAL_NULL(&property_phase_default_value);
	zend_string *property_phase_name = zend_string_init("phase", sizeof("phase") - 1, 1);
	zend_declare_typed_property(class_entry, property_phase_name, &property_phase_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_DOUBLE|MAY_BE_NULL));
	zend_string_release(property_phase_name);

	return class_entry;
}
