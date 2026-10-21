/*
 * exceptions.h
 *
 * Registers OceanMoon\Core\Exceptions\FormatException and ArithmeticException as far as the math
 * extension needs them (both are trivial empty subclasses of the built-in DomainException in the
 * PHP source - no methods or properties of their own to port). Not the whole of
 * OceanMoon\Core\Exceptions, just the two custom exception types Complex/Rational/Vector/Matrix
 * actually throw.
 *
 * Registered against spl_ce_DomainException, PHP's own built-in class entry, so no other Core
 * class needs to exist natively for these to work - same reasoning as floats.h/integers.h being
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
 * Called from PHP_MINIT_FUNCTION(oceanmoon_math) in oceanmoon_math.c, before any class that might throw either
 * exception is registered.
 */
zend_result math_exceptions_minit(void);
/* }}} */

/* {{{ math_throw_overflow
 *
 * Throws OverflowException "Cannot <operation>. The result would overflow.", matching the PHP package's overflow
 * messages. If an exception is already pending, the engine chains it as the new exception's previous.
 */
void math_throw_overflow(const char *operation);
/* }}} */

/* {{{ math_throw_scalar_overflow
 *
 * math_throw_overflow() for an operation with a scalar operand. format has one %s, for the scalar, e.g. "multiply the
 * given Matrix by %s". Formatting the message only when it's thrown saves the cost on every successful call.
 */
void math_throw_scalar_overflow(const char *format, double scalar);
/* }}} */

/* {{{ math_rethrow_overflow
 *
 * If the pending exception is an OverflowException from an intermediate step, replaces it with one naming the
 * operation the caller ran, chaining the original as its previous. Any other pending exception is left alone.
 * Matches the PHP package catching OverflowException and rethrowing with its own message.
 */
void math_rethrow_overflow(const char *operation);
/* }}} */

/* {{{ math_abbrev_string
 *
 * Matches OceanMoon\Core\Stringify::abbrev() for a string, as Stringify::prepEx() shows it in an exception message:
 * single-quoted, with backslashes and single quotes escaped, and cut to 32 characters, ending "…'", if longer.
 * Returns NULL if the string isn't valid UTF-8, which Stringify can't show. The caller releases the result.
 */
zend_string *math_abbrev_string(const char *str, size_t len);
/* }}} */

/* {{{ math_throw_string_error
 *
 * Throws ce with the PHP package's message for a string that can't be converted, e.g. "Cannot convert string '1e400'
 * to Rational. Outside valid range.", with the string abbreviated by math_abbrev_string(). A string that isn't valid
 * UTF-8 is left out of the message. A NUL in the string is kept, as in the package.
 */
void math_throw_string_error(
	zend_class_entry *ce, const char *str, size_t len, const char *class_name, const char *reason
);
/* }}} */

/* {{{ math_throw_invalid_format
 *
 * Throws the PHP package's fromString() FormatException for an unparseable string, e.g. "Cannot convert string
 * '3+4x' to Complex. Invalid format.", via math_throw_string_error().
 */
void math_throw_invalid_format(const char *str, size_t len, const char *class_name);
/* }}} */

#endif /* PHP_MATH_EXCEPTIONS_H */
