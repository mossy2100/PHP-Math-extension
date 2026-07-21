/*
 * floats.h
 *
 * Shared floating-point helpers, not specific to any one class -- used by Complex's
 * approxEqual() today, and intended for Rational/Vector/Matrix's equivalents later.
 */

#ifndef PHP_MATH_FLOATS_H
#define PHP_MATH_FLOATS_H

#include <stdbool.h>

#include "php.h"

/* {{{ math_floats_approx_equal
 *
 * Ports OceanMoon\Core\Floats::approxEqual(): checks whether two doubles are equal within a
 * combined absolute/relative tolerance, matching Python's math.isclose().
 *
 *  - NaN is never approximately equal to anything, including itself.
 *  - If either value is infinite, falls back to exact equality (tolerance math doesn't apply).
 *  - Otherwise: exact equality short-circuits to true; then the absolute difference is checked
 *    against abs_tol (useful near zero); if that fails, it's checked against
 *    rel_tol * max(|a|, |b|) (scales with magnitude).
 *
 * Deliberately does NOT validate rel_tol/abs_tol for negativity itself (the PHP method does, and
 * throws DomainException) -- like the rest of this file, it's pure math with no PHP/exception
 * awareness. Callers with negative tolerances to reject should check before calling this.
 */
bool math_floats_approx_equal(double a, double b, double rel_tol, double abs_tol);
/* }}} */

/* {{{ math_floats_normalize_zero
 *
 * Ports OceanMoon\Core\Floats::normalizeZero(): collapses -0.0 to 0.0, leaving every other value
 * (including NaN and the infinities) unchanged.
 */
double math_floats_normalize_zero(double value);
/* }}} */

/* {{{ math_floats_wrap
 *
 * Ports OceanMoon\Core\Floats::wrap(): reduces value modulo units_per_turn, then shifts it into
 * range -- either the signed range (-units_per_turn/2, units_per_turn/2] if `signed_range` is
 * true, or the unsigned range [0, units_per_turn) if false. The result is passed through
 * math_floats_normalize_zero() before returning, so -0.0 never comes out even if it would
 * otherwise (e.g. wrapping a value that lands exactly on 0 from the negative side).
 */
double math_floats_wrap(double value, double units_per_turn, bool signed_range);
/* }}} */

/* {{{ math_floats_is_int
 *
 * Ports OceanMoon\Core\Floats::isInt(): true if value is finite and has no fractional part (i.e.
 * floor(value) == value). False for NaN, +/-INF, and any value with a fractional component.
 */
bool math_floats_is_int(double value);
/* }}} */

/* {{{ math_floats_to_int
 *
 * Ports OceanMoon\Core\Floats::toInt(): attempts to convert f to a zend_long losslessly. Returns
 * true and sets *out on success; returns false (leaving *out untouched) if f is not an
 * math_floats_is_int() value, or is outside the representable zend_long range.
 *
 * Like math_floats_approx_equal(), this is pure math with no PHP/exception awareness -- the PHP
 * method throws DomainException on failure, but that's the caller's responsibility here (using
 * whatever value-formatting the caller already has on hand for the message, e.g. to safely
 * stringify NAN/INF).
 *
 * PHP_INT_MIN is exactly representable as a double (it's a power of two), but PHP_INT_MAX is not
 * -- the nearest representable double above it rounds up past the valid range. The comparison
 * therefore uses `f < -limit` (strictly less than the negation of the min, i.e. the double closest
 * to but not reaching PHP_INT_MAX + 1) rather than `f <= (double) ZEND_LONG_MAX`, mirroring the PHP
 * implementation's own comment about this exactly.
 */
bool math_floats_to_int(double f, zend_long *out);
/* }}} */

/* {{{ math_floats_to_str
 *
 * Ports OceanMoon\Core\Stringify::stringifyFloat(): NAN/+INF/-INF stringify to their literal
 * names ("NAN", "INF", "-INF"); every other (finite) value goes through the engine's normal
 * (string) cast machinery, for byte-for-byte parity with what `(string) $value` would produce in
 * PHP (governed by the `precision` ini setting). Used for embedding a float value in an exception
 * message.
 *
 * Returns a new zend_string the caller owns and must eventually release.
 */
zend_string *math_floats_to_str(double value);
/* }}} */

#endif /* PHP_MATH_FLOATS_H */
