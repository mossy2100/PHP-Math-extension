PHP_ARG_ENABLE([oceanmoon_math],
  [whether to enable oceanmoon_math support],
  [AS_HELP_STRING([--enable-oceanmoon_math],
    [Enable OceanMoon math support])],
  [no])

if test "$PHP_OCEANMOON_MATH" != "no"; then
  AC_DEFINE(HAVE_OCEANMOON_MATH, 1, [ Have OceanMoon math support ])
  PHP_NEW_EXTENSION(oceanmoon_math, math.c floats.c integers.c types.c exceptions.c Complex/complex.c Complex/complex_conversion.c Complex/complex_inspection.c Complex/complex_comparison.c Complex/complex_properties.c Complex/complex_arithmetic.c Complex/complex_transcendental.c Complex/complex_power.c Complex/complex_operators.c Complex/complex_rounding.c Complex/complex_trigonometric.c Complex/complex_hyperbolic.c Complex/complex_arrayaccess.c Rational/rational.c Rational/rational_factory.c Rational/rational_conversion.c Rational/rational_comparison.c Rational/rational_arithmetic.c Rational/rational_power.c Rational/rational_rounding.c Rational/rational_operators.c Vector/vector.c Vector/vector_factory.c Vector/vector_properties.c Vector/vector_conversion.c Vector/vector_inspection.c Vector/vector_modification.c Vector/vector_comparison.c Vector/vector_arithmetic.c Vector/vector_linear_algebra.c Vector/vector_aggregation.c Vector/vector_arrayaccess.c Vector/vector_operators.c Matrix/matrix.c Matrix/matrix_factory.c Matrix/matrix_conversion.c Matrix/matrix_inspection.c Matrix/matrix_modification.c Matrix/matrix_comparison.c Matrix/matrix_transformation.c Matrix/matrix_arithmetic.c Matrix/matrix_power.c Matrix/matrix_linear_algebra.c Matrix/matrix_norm.c Matrix/matrix_aggregation.c Matrix/matrix_arrayaccess.c Matrix/matrix_helpers.c Matrix/matrix_operators.c, $ext_shared)
fi
