/*
 * floats.h
 *
 * Shared floating-point helpers, not specific to any one class -- used by Complex's
 * approxEqual() today, and intended for Rational/Vector/Matrix's equivalents later.
 */

#ifndef PHP_MATH_FLOATS_H
#define PHP_MATH_FLOATS_H

#include <stdbool.h>

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
 */
bool math_floats_approx_equal(double a, double b, double rel_tol, double abs_tol);
/* }}} */

#endif /* PHP_MATH_FLOATS_H */
