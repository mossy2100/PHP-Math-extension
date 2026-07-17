/*
 * exceptions.h
 *
 * Registers OceanMoon\Core\Exceptions\FormatException and ArithmeticException as far as the math
 * extension needs them (both are trivial empty subclasses of the built-in DomainException in the
 * PHP source -- no methods or properties of their own to port). Not the whole of
 * OceanMoon\Core\Exceptions, just the two custom exception types Complex/Rational/Vector/Matrix
 * actually throw.
 *
 * Registered against spl_ce_DomainException, PHP's own built-in class entry, so no other Core
 * class needs to exist natively for these to work -- same reasoning as floats.h/integers.h being
 * pure, standalone helpers with no dependency on a ported OceanMoon\Core class.
 */

#ifndef PHP_MATH_EXCEPTIONS_H
#define PHP_MATH_EXCEPTIONS_H

#include "php.h"

/* The registered class entries. Defined in exceptions.c. */
extern zend_class_entry *math_ce_FormatException;
extern zend_class_entry *math_ce_ArithmeticException;

/* {{{ math_exceptions_minit
 *
 * Called from PHP_MINIT_FUNCTION(math) in math.c, before any class that might throw either
 * exception is registered.
 */
zend_result math_exceptions_minit(void);
/* }}} */

#endif /* PHP_MATH_EXCEPTIONS_H */
