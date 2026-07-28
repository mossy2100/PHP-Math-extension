/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 8fd78e57d4e24e1234313353ebc6003d2435f95b */

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

#define arginfo_class_OceanMoon_Math_Complex_sin arginfo_class_OceanMoon_Math_Complex_neg

#define arginfo_class_OceanMoon_Math_Complex_cos arginfo_class_OceanMoon_Math_Complex_neg

#define arginfo_class_OceanMoon_Math_Complex_tan arginfo_class_OceanMoon_Math_Complex_neg

#define arginfo_class_OceanMoon_Math_Complex_asin arginfo_class_OceanMoon_Math_Complex_neg

#define arginfo_class_OceanMoon_Math_Complex_acos arginfo_class_OceanMoon_Math_Complex_neg

#define arginfo_class_OceanMoon_Math_Complex_atan arginfo_class_OceanMoon_Math_Complex_neg

#define arginfo_class_OceanMoon_Math_Complex_sinh arginfo_class_OceanMoon_Math_Complex_neg

#define arginfo_class_OceanMoon_Math_Complex_cosh arginfo_class_OceanMoon_Math_Complex_neg

#define arginfo_class_OceanMoon_Math_Complex_tanh arginfo_class_OceanMoon_Math_Complex_neg

#define arginfo_class_OceanMoon_Math_Complex_asinh arginfo_class_OceanMoon_Math_Complex_neg

#define arginfo_class_OceanMoon_Math_Complex_acosh arginfo_class_OceanMoon_Math_Complex_neg

#define arginfo_class_OceanMoon_Math_Complex_atanh arginfo_class_OceanMoon_Math_Complex_neg

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Complex_round, 0, 0, OceanMoon\\Math\\Complex, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, precision, IS_LONG, 0, "0")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, mode, RoundingMode, 0, "RoundingMode::HalfAwayFromZero")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Complex_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Complex_offsetGet, 0, 1, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Complex_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Complex_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

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

#define arginfo_class_OceanMoon_Math_Rational___toString arginfo_class_OceanMoon_Math_Complex___toString

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Rational_compare, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, other, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Rational_equal arginfo_class_OceanMoon_Math_Complex_equal

#define arginfo_class_OceanMoon_Math_Rational_lessThan arginfo_class_OceanMoon_Math_Complex_equal

#define arginfo_class_OceanMoon_Math_Rational_lessThanOrEqual arginfo_class_OceanMoon_Math_Complex_equal

#define arginfo_class_OceanMoon_Math_Rational_greaterThan arginfo_class_OceanMoon_Math_Complex_equal

#define arginfo_class_OceanMoon_Math_Rational_greaterThanOrEqual arginfo_class_OceanMoon_Math_Complex_equal

#define arginfo_class_OceanMoon_Math_Rational_approxEqual arginfo_class_OceanMoon_Math_Complex_approxEqual

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

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Rational_pow, 0, 1, OceanMoon\\Math\\Rational, 0)
	ZEND_ARG_TYPE_INFO(0, exponent, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Rational_sqr arginfo_class_OceanMoon_Math_Rational_abs

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Rational_round, 0, 0, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, mode, RoundingMode, 0, "RoundingMode::HalfAwayFromZero")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Rational_floor, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Rational_ceil arginfo_class_OceanMoon_Math_Rational_floor

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OceanMoon_Math_Vector___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, count, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Vector_fromArray, 0, 1, OceanMoon\\Math\\Vector, 0)
	ZEND_ARG_TYPE_INFO(0, arr, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Vector_toArray arginfo_class_OceanMoon_Math_Rational_toMixedNumber

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Vector_toRowMatrix, 0, 0, OceanMoon\\Math\\Matrix, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Vector_toColumnMatrix arginfo_class_OceanMoon_Math_Vector_toRowMatrix

#define arginfo_class_OceanMoon_Math_Vector___toString arginfo_class_OceanMoon_Math_Complex___toString

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Vector_get, 0, 1, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Vector_set, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OceanMoon_Math_Vector_normalize, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Vector_equal arginfo_class_OceanMoon_Math_Complex_equal

#define arginfo_class_OceanMoon_Math_Vector_approxEqual arginfo_class_OceanMoon_Math_Complex_approxEqual

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

#define arginfo_class_OceanMoon_Math_Vector_sum arginfo_class_OceanMoon_Math_Rational_toFloat

#define arginfo_class_OceanMoon_Math_Vector_prod arginfo_class_OceanMoon_Math_Rational_toFloat

#define arginfo_class_OceanMoon_Math_Vector_count arginfo_class_OceanMoon_Math_Rational_floor

#define arginfo_class_OceanMoon_Math_Vector_offsetExists arginfo_class_OceanMoon_Math_Complex_offsetExists

#define arginfo_class_OceanMoon_Math_Vector_offsetGet arginfo_class_OceanMoon_Math_Complex_offsetGet

#define arginfo_class_OceanMoon_Math_Vector_offsetSet arginfo_class_OceanMoon_Math_Complex_offsetSet

#define arginfo_class_OceanMoon_Math_Vector_offsetUnset arginfo_class_OceanMoon_Math_Complex_offsetUnset

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

#define arginfo_class_OceanMoon_Math_Matrix_toArray arginfo_class_OceanMoon_Math_Rational_toMixedNumber

#define arginfo_class_OceanMoon_Math_Matrix___toString arginfo_class_OceanMoon_Math_Complex___toString

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

#define arginfo_class_OceanMoon_Math_Matrix_equal arginfo_class_OceanMoon_Math_Complex_equal

#define arginfo_class_OceanMoon_Math_Matrix_approxEqual arginfo_class_OceanMoon_Math_Complex_approxEqual

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_resize, 0, 2, OceanMoon\\Math\\Matrix, 0)
	ZEND_ARG_TYPE_INFO(0, rowCount, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, columnCount, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Matrix_neg arginfo_class_OceanMoon_Math_Vector_toRowMatrix

#define arginfo_class_OceanMoon_Math_Matrix_reciprocal arginfo_class_OceanMoon_Math_Vector_toRowMatrix

#define arginfo_class_OceanMoon_Math_Matrix_inv arginfo_class_OceanMoon_Math_Vector_toRowMatrix

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

#define arginfo_class_OceanMoon_Math_Matrix_sqr arginfo_class_OceanMoon_Math_Vector_toRowMatrix

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_mulVector, 0, 1, OceanMoon\\Math\\Vector, 0)
	ZEND_ARG_OBJ_INFO(0, vector, OceanMoon\\Math\\Vector, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Matrix_t arginfo_class_OceanMoon_Math_Vector_toRowMatrix

#define arginfo_class_OceanMoon_Math_Matrix_det arginfo_class_OceanMoon_Math_Rational_toFloat

#define arginfo_class_OceanMoon_Math_Matrix_trace arginfo_class_OceanMoon_Math_Rational_toFloat

#define arginfo_class_OceanMoon_Math_Matrix_norm arginfo_class_OceanMoon_Math_Rational_toFloat

#define arginfo_class_OceanMoon_Math_Matrix_p1Norm arginfo_class_OceanMoon_Math_Rational_toFloat

#define arginfo_class_OceanMoon_Math_Matrix_pInfNorm arginfo_class_OceanMoon_Math_Rational_toFloat

#define arginfo_class_OceanMoon_Math_Matrix_count arginfo_class_OceanMoon_Math_Rational_floor

#define arginfo_class_OceanMoon_Math_Matrix_offsetExists arginfo_class_OceanMoon_Math_Complex_offsetExists

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OceanMoon_Math_Matrix_offsetGet, 0, 1, OceanMoon\\Math\\Vector, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OceanMoon_Math_Matrix_offsetSet arginfo_class_OceanMoon_Math_Complex_offsetSet

#define arginfo_class_OceanMoon_Math_Matrix_offsetUnset arginfo_class_OceanMoon_Math_Complex_offsetUnset

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
ZEND_METHOD(OceanMoon_Math_Complex, sin);
ZEND_METHOD(OceanMoon_Math_Complex, cos);
ZEND_METHOD(OceanMoon_Math_Complex, tan);
ZEND_METHOD(OceanMoon_Math_Complex, asin);
ZEND_METHOD(OceanMoon_Math_Complex, acos);
ZEND_METHOD(OceanMoon_Math_Complex, atan);
ZEND_METHOD(OceanMoon_Math_Complex, sinh);
ZEND_METHOD(OceanMoon_Math_Complex, cosh);
ZEND_METHOD(OceanMoon_Math_Complex, tanh);
ZEND_METHOD(OceanMoon_Math_Complex, asinh);
ZEND_METHOD(OceanMoon_Math_Complex, acosh);
ZEND_METHOD(OceanMoon_Math_Complex, atanh);
ZEND_METHOD(OceanMoon_Math_Complex, round);
ZEND_METHOD(OceanMoon_Math_Complex, offsetExists);
ZEND_METHOD(OceanMoon_Math_Complex, offsetGet);
ZEND_METHOD(OceanMoon_Math_Complex, offsetSet);
ZEND_METHOD(OceanMoon_Math_Complex, offsetUnset);
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
ZEND_METHOD(OceanMoon_Math_Rational, pow);
ZEND_METHOD(OceanMoon_Math_Rational, sqr);
ZEND_METHOD(OceanMoon_Math_Rational, round);
ZEND_METHOD(OceanMoon_Math_Rational, floor);
ZEND_METHOD(OceanMoon_Math_Rational, ceil);
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
ZEND_METHOD(OceanMoon_Math_Matrix, mulVector);
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
	ZEND_ME(OceanMoon_Math_Complex, sin, arginfo_class_OceanMoon_Math_Complex_sin, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, cos, arginfo_class_OceanMoon_Math_Complex_cos, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, tan, arginfo_class_OceanMoon_Math_Complex_tan, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, asin, arginfo_class_OceanMoon_Math_Complex_asin, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, acos, arginfo_class_OceanMoon_Math_Complex_acos, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, atan, arginfo_class_OceanMoon_Math_Complex_atan, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, sinh, arginfo_class_OceanMoon_Math_Complex_sinh, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, cosh, arginfo_class_OceanMoon_Math_Complex_cosh, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, tanh, arginfo_class_OceanMoon_Math_Complex_tanh, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, asinh, arginfo_class_OceanMoon_Math_Complex_asinh, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, acosh, arginfo_class_OceanMoon_Math_Complex_acosh, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, atanh, arginfo_class_OceanMoon_Math_Complex_atanh, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, round, arginfo_class_OceanMoon_Math_Complex_round, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, offsetExists, arginfo_class_OceanMoon_Math_Complex_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, offsetGet, arginfo_class_OceanMoon_Math_Complex_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, offsetSet, arginfo_class_OceanMoon_Math_Complex_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Complex, offsetUnset, arginfo_class_OceanMoon_Math_Complex_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

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
	ZEND_ME(OceanMoon_Math_Rational, pow, arginfo_class_OceanMoon_Math_Rational_pow, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Rational, sqr, arginfo_class_OceanMoon_Math_Rational_sqr, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Rational, round, arginfo_class_OceanMoon_Math_Rational_round, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Rational, floor, arginfo_class_OceanMoon_Math_Rational_floor, ZEND_ACC_PUBLIC)
	ZEND_ME(OceanMoon_Math_Rational, ceil, arginfo_class_OceanMoon_Math_Rational_ceil, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

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
	ZEND_ME(OceanMoon_Math_Matrix, mulVector, arginfo_class_OceanMoon_Math_Matrix_mulVector, ZEND_ACC_PUBLIC)
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

static zend_class_entry *register_class_OceanMoon_Math_Complex(zend_class_entry *class_entry_Stringable, zend_class_entry *class_entry_ArrayAccess)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "OceanMoon\\Math", "Complex", class_OceanMoon_Math_Complex_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);
	zend_class_implements(class_entry, 2, class_entry_Stringable, class_entry_ArrayAccess);

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
	ZVAL_DOUBLE(&property_magnitude_default_value, 0.0);
	zend_string *property_magnitude_name = zend_string_init("magnitude", sizeof("magnitude") - 1, 1);
	zend_declare_typed_property(class_entry, property_magnitude_name, &property_magnitude_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_DOUBLE));
	zend_string_release(property_magnitude_name);

	zval property_phase_default_value;
	ZVAL_DOUBLE(&property_phase_default_value, 0.0);
	zend_string *property_phase_name = zend_string_init("phase", sizeof("phase") - 1, 1);
	zend_declare_typed_property(class_entry, property_phase_name, &property_phase_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_DOUBLE));
	zend_string_release(property_phase_name);

	return class_entry;
}

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

static zend_class_entry *register_class_OceanMoon_Math_Vector(zend_class_entry *class_entry_Stringable, zend_class_entry *class_entry_Countable, zend_class_entry *class_entry_ArrayAccess)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "OceanMoon\\Math", "Vector", class_OceanMoon_Math_Vector_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);
	zend_class_implements(class_entry, 3, class_entry_Stringable, class_entry_Countable, class_entry_ArrayAccess);

	zval property_count_default_value;
	ZVAL_LONG(&property_count_default_value, 0);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_COUNT), &property_count_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));

	zval property_data_default_value;
	ZVAL_EMPTY_ARRAY(&property_data_default_value);
	zend_string *property_data_name = zend_string_init("data", sizeof("data") - 1, 1);
	zend_declare_typed_property(class_entry, property_data_name, &property_data_default_value, ZEND_ACC_PRIVATE, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ARRAY));
	zend_string_release(property_data_name);

	zval property_magnitude_default_value;
	ZVAL_UNDEF(&property_magnitude_default_value);
	zend_string *property_magnitude_name = zend_string_init("magnitude", sizeof("magnitude") - 1, 1);
	zend_declare_typed_property(class_entry, property_magnitude_name, &property_magnitude_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_DOUBLE));
	zend_string_release(property_magnitude_name);

	return class_entry;
}

static zend_class_entry *register_class_OceanMoon_Math_Matrix(zend_class_entry *class_entry_Stringable, zend_class_entry *class_entry_Countable, zend_class_entry *class_entry_ArrayAccess)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "OceanMoon\\Math", "Matrix", class_OceanMoon_Math_Matrix_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);
	zend_class_implements(class_entry, 3, class_entry_Stringable, class_entry_Countable, class_entry_ArrayAccess);

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

	zval property_data_default_value;
	ZVAL_EMPTY_ARRAY(&property_data_default_value);
	zend_string *property_data_name = zend_string_init("data", sizeof("data") - 1, 1);
	zend_declare_typed_property(class_entry, property_data_name, &property_data_default_value, ZEND_ACC_PRIVATE, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ARRAY));
	zend_string_release(property_data_name);

	return class_entry;
}
