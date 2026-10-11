PHP_ARG_ENABLE([oceanmoon_math],
  [whether to enable oceanmoon_math support],
  [AS_HELP_STRING([--enable-oceanmoon_math],
    [Enable OceanMoon math support])],
  [no])

if test "$PHP_OCEANMOON_MATH" != "no"; then
  AC_DEFINE(HAVE_OCEANMOON_MATH, 1, [ Have OceanMoon math support ])
  dnl Never fuse a multiply and an add into one FMA instruction. PHP rounds every operation separately, so fusing
  dnl breaks parity with the package: a*b - c*d is no longer exactly 0 when a*b == c*d (e.g. Vector::cross()).
  dnl Likewise, never combine sin(x) and cos(x) into one sincos call. On macOS, __sincos_stret can round differently
  dnl from the sin() PHP calls (e.g. Complex::fromPolar()).
  PHP_NEW_EXTENSION(oceanmoon_math, oceanmoon_math.c src/floats.c src/integers.c src/types.c src/exceptions.c src/Complex/complex.c src/Complex/complex_conversion.c src/Complex/complex_inspection.c src/Complex/complex_comparison.c src/Complex/complex_properties.c src/Complex/complex_arithmetic.c src/Complex/complex_transcendental.c src/Complex/complex_power.c src/Complex/complex_operators.c src/Complex/complex_rounding.c src/Complex/complex_trigonometric.c src/Complex/complex_hyperbolic.c src/Complex/complex_arrayaccess.c src/Rational/rational.c src/Rational/rational_factory.c src/Rational/rational_conversion.c src/Rational/rational_inspection.c src/Rational/rational_comparison.c src/Rational/rational_arithmetic.c src/Rational/rational_power.c src/Rational/rational_rounding.c src/Rational/rational_operators.c src/Vector/vector.c src/Vector/vector_factory.c src/Vector/vector_properties.c src/Vector/vector_conversion.c src/Vector/vector_inspection.c src/Vector/vector_modification.c src/Vector/vector_comparison.c src/Vector/vector_arithmetic.c src/Vector/vector_linear_algebra.c src/Vector/vector_aggregation.c src/Vector/vector_arrayaccess.c src/Vector/vector_operators.c src/Matrix/matrix.c src/Matrix/matrix_factory.c src/Matrix/matrix_conversion.c src/Matrix/matrix_inspection.c src/Matrix/matrix_modification.c src/Matrix/matrix_comparison.c src/Matrix/matrix_transformation.c src/Matrix/matrix_arithmetic.c src/Matrix/matrix_power.c src/Matrix/matrix_linear_algebra.c src/Matrix/matrix_norm.c src/Matrix/matrix_aggregation.c src/Matrix/matrix_arrayaccess.c src/Matrix/matrix_helpers.c src/Matrix/matrix_operators.c, $ext_shared, , -ffp-contract=off -fno-builtin-sin -fno-builtin-cos)
  PHP_ADD_INCLUDE([$ext_srcdir/src])
fi
