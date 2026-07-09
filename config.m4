PHP_ARG_ENABLE([complex],
  [whether to enable complex support],
  [AS_HELP_STRING([--enable-complex],
    [Enable OceanMoon complex number support])],
  [no])

if test "$PHP_COMPLEX" != "no"; then
  AC_DEFINE(HAVE_COMPLEX, 1, [ Have OceanMoon complex support ])
  PHP_NEW_EXTENSION(complex, complex.c, $ext_shared)
fi
