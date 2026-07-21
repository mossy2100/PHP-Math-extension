/*
 * rational.c
 *
 * OceanMoon\Math\Rational: class registration, the constructor, and the shared internal helpers
 * (rational_init/rational_create/rational_read_parts) declared in rational_internal.h and used by
 * this file as well as the other rational_*.c implementation files in this directory (none yet --
 * this is the first slice, mirroring how Complex/complex.c started).
 *
 * rational_minit() is called from the module's real PHP_MINIT_FUNCTION in ../math.c, which owns
 * module lifecycle for the whole `math` extension.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "rational_internal.h"
#include "rational_arginfo.h"
#include "exceptions.h"
#include "integers.h"

/* The registered class entry for OceanMoon\Math\Rational. */
zend_class_entry *rational_ce_Rational;

/* {{{ rational_init
 *
 * Validate num/den and set numerator/denominator (in canonical, reduced form) on an
 * already-allocated Rational object. Shared by __construct() and rational_create() (used by
 * future from*() factory methods), mirroring how the PHP implementation's factory methods all
 * funnel through `new self(...)`, so the canonicalization logic lives in exactly one place.
 *
 * Ports the PHP source's __construct() body exactly, with two C-specific safety adjustments
 * neither needed in PHP:
 *
 *  - `den == ZEND_LONG_MIN && num == -den`: evaluating `-den` when den is ZEND_LONG_MIN is
 *    undefined behavior in C (negating the minimum representable value overflows). PHP's
 *    equivalent `$num === -$den` never actually matches in this case anyway -- PHP silently
 *    promotes `-PHP_INT_MIN` to a float, so the strict `===` against an int `$num` is always
 *    false by type mismatch alone -- so guarding this comparison with `den != ZEND_LONG_MIN &&`
 *    to skip it entirely reproduces the same "never matches" outcome without the UB.
 *  - The final `if ($den < 0) { $num = -$num; $den = -$den; }` negations are safe here because,
 *    by this point, neither value can still be ZEND_LONG_MIN: the only way either could have
 *    started as ZEND_LONG_MIN is if the *other* value was even (checked above), which guarantees
 *    math_integers_gcd() below returns a divisor >= 2, so dividing by it always reduces a
 *    ZEND_LONG_MIN operand to a normal-range value before this point is reached.
 *
 * Returns FAILURE (with an exception already thrown) if den is zero, or if the ratio can't be
 * represented (one of num/den is ZEND_LONG_MIN and the other is odd -- ZEND_LONG_MIN can't be
 * safely negated, which the reduction/normalization steps below would otherwise require).
 */
zend_result rational_init(zend_object *obj, zend_long num, zend_long den)
{
	if (den == 0) {
		zend_throw_exception(math_ce_ArithmeticException, "Cannot create Rational with denominator of zero.", 0);
		return FAILURE;
	}

	if ((num == ZEND_LONG_MIN && den % 2 != 0) || (den == ZEND_LONG_MIN && num % 2 != 0)) {
		zend_string *msg = strpprintf(
			0, "Cannot express ratio " ZEND_LONG_FMT "/" ZEND_LONG_FMT " as Rational.", num, den);
		zend_throw_exception(spl_ce_DomainException, ZSTR_VAL(msg), 0);
		zend_string_release(msg);
		return FAILURE;
	}

	if (num == 0) {
		/* A zero numerator is always represented as 0/1. */
		den = 1;
	} else if (num == den) {
		/* The numerator and denominator are equal to each other. */
		num = 1;
		den = 1;
	} else if (den != ZEND_LONG_MIN && num == -den) {
		/* The numerator is equal to the negative of the denominator. */
		num = -1;
		den = 1;
	} else {
		/* Calculate the GCD. */
		zend_long nums[2] = {num, den};
		zend_long gcd;
		math_integers_result gcd_result = math_integers_gcd(nums, 2, &gcd);
		if (gcd_result != MATH_INTEGERS_OK) {
			/* Not reachable: the branches above already exclude every input combination that
			 * could make the true GCD unrepresentable (num == 0, den == 0, and num/den both
			 * ZEND_LONG_MIN are all handled before this point). Guarded anyway rather than
			 * trusting that reasoning silently at runtime. */
			zend_throw_exception(spl_ce_DomainException, "Cannot reduce Rational to canonical form.", 0);
			return FAILURE;
		}

		/* Reduce the fraction if necessary. */
		if (gcd > 1) {
			num = num / gcd;
			den = den / gcd;
		}

		/* Ensure the denominator is positive. */
		if (den < 0) {
			num = -num;
			den = -den;
		}
	}

	zend_update_property_long(rational_ce_Rational, obj, "numerator", sizeof("numerator") - 1, num);
	zend_update_property_long(rational_ce_Rational, obj, "denominator", sizeof("denominator") - 1, den);

	return SUCCESS;
}
/* }}} */

/* {{{ rational_create
 *
 * Allocate a new Rational object into return_value and initialize it via rational_init(). Used by
 * all the from*() factory paths (once they exist).
 */
zend_result rational_create(zval *return_value, zend_long num, zend_long den)
{
	object_init_ex(return_value, rational_ce_Rational);
	return rational_init(Z_OBJ_P(return_value), num, den);
}
/* }}} */

/* {{{ rational_read_parts
 *
 * Reads the numerator and denominator off a zend_object already known to be a Rational instance.
 * Shared by every rational_*.c implementation file (once they exist).
 */
void rational_read_parts(zend_object *obj, zend_long *out_num, zend_long *out_den)
{
	zval rv1, rv2;
	*out_num = zval_get_long(
		zend_read_property(rational_ce_Rational, obj, "numerator", sizeof("numerator") - 1, 1, &rv1));
	*out_den = zval_get_long(
		zend_read_property(rational_ce_Rational, obj, "denominator", sizeof("denominator") - 1, 1, &rv2));
}
/* }}} */

/* {{{ OceanMoon\Math\Rational::__construct(int $num = 0, int $den = 1) */
PHP_METHOD(OceanMoon_Math_Rational, __construct)
{
	zend_long num = 0, den = 1;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(num)
		Z_PARAM_LONG(den)
	ZEND_PARSE_PARAMETERS_END();

	if (rational_init(Z_OBJ_P(ZEND_THIS), num, den) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ rational_minit
 *
 * Called from PHP_MINIT_FUNCTION(math) in ../math.c. Class entry, method table, and typed
 * properties are all generated from rational.stub.php into rational_arginfo.h; register them
 * here.
 */
zend_result rational_minit(void)
{
	rational_ce_Rational = register_class_OceanMoon_Math_Rational();

	return SUCCESS;
}
/* }}} */
