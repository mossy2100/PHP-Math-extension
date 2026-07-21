PHP_ARG_ENABLE([oceanmoon_math],
  [whether to enable oceanmoon_math support],
  [AS_HELP_STRING([--enable-oceanmoon_math],
    [Enable OceanMoon math support])],
  [no])

if test "$PHP_OCEANMOON_MATH" != "no"; then
  AC_DEFINE(HAVE_OCEANMOON_MATH, 1, [ Have OceanMoon math support ])
  PHP_NEW_EXTENSION(oceanmoon_math, math.c floats.c integers.c types.c exceptions.c Complex/complex.c Complex/complex_conversion.c Complex/complex_inspection.c Complex/complex_comparison.c Complex/complex_properties.c Complex/complex_arithmetic.c Complex/complex_transcendental.c Complex/complex_power.c Complex/complex_operators.c Complex/complex_rounding.c Complex/complex_trigonometric.c Complex/complex_hyperbolic.c Complex/complex_arrayaccess.c Rational/rational.c Rational/rational_factory.c Rational/rational_conversion.c Rational/rational_comparison.c Rational/rational_arithmetic.c, $ext_shared)
fi
