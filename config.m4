PHP_ARG_ENABLE([math],
  [whether to enable math support],
  [AS_HELP_STRING([--enable-math],
    [Enable OceanMoon math support])],
  [no])

if test "$PHP_MATH" != "no"; then
  AC_DEFINE(HAVE_MATH, 1, [ Have OceanMoon math support ])
  PHP_NEW_EXTENSION(math, math.c floats.c integers.c exceptions.c Complex/complex.c Complex/complex_conversion.c Complex/complex_inspection.c Complex/complex_comparison.c, $ext_shared)
fi
