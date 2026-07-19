/*
 * complex_arithmetic.c
 *
 * Unary and binary arithmetic methods for OceanMoon\Math\Complex: neg(), inv(), conj(), add(),
 * sub(), mul(), div(). Mirrors the "Unary arithmetic methods"/"Binary arithmetic methods" regions
 * of the PHP package's Complex class.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "complex_internal.h"
#include "complex_arginfo.h"
#include "exceptions.h"

/* {{{ complex_read_other
 *
 * Shared by add()/sub()/mul()/div(): resolves $other (declared `Complex|float` in the stub) into a
 * (real, imaginary) pair. Unlike a userland-typed parameter, an internal function's zval parameter
 * isn't automatically coerced or validated against its declared arginfo type before the function
 * body runs, so both `int` (PHP's usual float-parameter widening) and `float` are accepted and
 * treated as a Complex with a zero imaginary part here -- matching the PHP package's
 * `new self($other)` conversion, including its finite-value validation. Anything else (including a
 * non-Complex object) throws TypeError.
 *
 * Returns FAILURE (with an exception already thrown) for a non-finite float/int or a wrong type.
 */
static zend_result complex_read_other(zval *other, double *out_real, double *out_imag)
{
	double d;

	if (Z_TYPE_P(other) == IS_LONG) {
		d = (double) Z_LVAL_P(other);
	} else if (Z_TYPE_P(other) == IS_DOUBLE) {
		d = Z_DVAL_P(other);
	} else if (Z_TYPE_P(other) == IS_OBJECT && instanceof_function(Z_OBJCE_P(other), complex_ce_Complex)) {
		complex_read_parts(Z_OBJ_P(other), out_real, out_imag);
		return SUCCESS;
	} else {
		zend_argument_type_error(1, "must be of type Complex|float, %s given", zend_zval_type_name(other));
		return FAILURE;
	}

	if (!zend_finite(d)) {
		zend_throw_exception(spl_ce_DomainException, "Cannot create a complex number from non-finite values.", 0);
		return FAILURE;
	}
	*out_real = d;
	*out_imag = 0.0;
	return SUCCESS;
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::neg(): Complex
 *
 * Matches the PHP package's Complex::neg().
 */
PHP_METHOD(OceanMoon_Math_Complex, neg)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double real, imag;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &real, &imag);

	if (complex_create(return_value, -real, -imag) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::inv(): Complex
 *
 * Matches the PHP package's Complex::inv() (1 / $this): 1/(c+di) = (c-di)/(c^2+d^2).
 */
PHP_METHOD(OceanMoon_Math_Complex, inv)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double real, imag;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &real, &imag);

	if (real == 0.0 && imag == 0.0) {
		zend_throw_exception(math_ce_ArithmeticException, "Cannot divide by zero.", 0);
		RETURN_THROWS();
	}

	double f = (real * real) + (imag * imag);
	if (complex_create(return_value, real / f, -imag / f) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::conj(): Complex
 *
 * Matches the PHP package's Complex::conj().
 */
PHP_METHOD(OceanMoon_Math_Complex, conj)
{
	ZEND_PARSE_PARAMETERS_NONE();

	double real, imag;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &real, &imag);

	if (complex_create(return_value, real, -imag) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::add(Complex|float $other): Complex
 *
 * Matches the PHP package's Complex::add().
 */
PHP_METHOD(OceanMoon_Math_Complex, add)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(other)
	ZEND_PARSE_PARAMETERS_END();

	double other_real, other_imag;
	if (complex_read_other(other, &other_real, &other_imag) == FAILURE) {
		RETURN_THROWS();
	}

	double real, imag;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &real, &imag);

	if (complex_create(return_value, real + other_real, imag + other_imag) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::sub(Complex|float $other): Complex
 *
 * Matches the PHP package's Complex::sub().
 */
PHP_METHOD(OceanMoon_Math_Complex, sub)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(other)
	ZEND_PARSE_PARAMETERS_END();

	double other_real, other_imag;
	if (complex_read_other(other, &other_real, &other_imag) == FAILURE) {
		RETURN_THROWS();
	}

	double real, imag;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &real, &imag);

	if (complex_create(return_value, real - other_real, imag - other_imag) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::mul(Complex|float $other): Complex
 *
 * Matches the PHP package's Complex::mul(): (a+bi)(c+di) = (ac-bd) + (ad+bc)i.
 */
PHP_METHOD(OceanMoon_Math_Complex, mul)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(other)
	ZEND_PARSE_PARAMETERS_END();

	double c, d;
	if (complex_read_other(other, &c, &d) == FAILURE) {
		RETURN_THROWS();
	}

	double a, b;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &a, &b);

	if (complex_create(return_value, a * c - b * d, a * d + b * c) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ OceanMoon\Math\Complex::div(Complex|float $other): Complex
 *
 * Matches the PHP package's Complex::div(): (a+bi)/(c+di) = [(ac+bd) + (bc-ad)i]/(c^2+d^2).
 */
PHP_METHOD(OceanMoon_Math_Complex, div)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(other)
	ZEND_PARSE_PARAMETERS_END();

	double c, d;
	if (complex_read_other(other, &c, &d) == FAILURE) {
		RETURN_THROWS();
	}

	if (c == 0.0 && d == 0.0) {
		zend_throw_exception(math_ce_ArithmeticException, "Cannot divide by zero.", 0);
		RETURN_THROWS();
	}

	double a, b;
	complex_read_parts(Z_OBJ_P(ZEND_THIS), &a, &b);

	double f = (c * c) + (d * d);
	if (complex_create(return_value, (a * c + b * d) / f, (b * c - a * d) / f) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */
