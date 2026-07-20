/*
 * complex_operators.c
 *
 * Operator overloading for OceanMoon\Math\Complex: +, -, *, /, ** (each accepting a Complex or
 * int|float operand on either side), plus unary - (negate) and ~ (conjugate). Mirrors
 * docs/Complex.md in this repo -- these operators only exist here in the extension; the userland
 * package has no way to overload operators, so it exposes the same behavior only as named methods
 * (add()/sub()/mul()/div()/pow()/neg()/conj()).
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

/* {{{ complex_operand_to_parts
 *
 * Resolves an operator operand (Complex, int, or float -- anything else is unsupported) into a
 * (real, imaginary) pair. Unlike complex_read_other() (complex_arithmetic.c), used for the named
 * methods, this never throws a TypeError for a wrong type -- it just returns FAILURE without an
 * exception, so the caller can return FAILURE from the do_operation handler in turn, letting the
 * engine fall through to its own standard "Unsupported operand types: Complex OP T" TypeError,
 * which is more idiomatic for operator misuse than a method-argument-shaped message. A non-finite
 * float/int still throws DomainException, matching add()/sub()/mul()/div()/pow()'s validation of
 * a scalar operand via `new self($other)`.
 */
static zend_result complex_operand_to_parts(zval *zv, double *out_real, double *out_imag)
{
	if (Z_TYPE_P(zv) == IS_OBJECT && instanceof_function(Z_OBJCE_P(zv), complex_ce_Complex)) {
		complex_read_parts(Z_OBJ_P(zv), out_real, out_imag);
		return SUCCESS;
	}

	double d;
	if (Z_TYPE_P(zv) == IS_LONG) {
		d = (double) Z_LVAL_P(zv);
	} else if (Z_TYPE_P(zv) == IS_DOUBLE) {
		d = Z_DVAL_P(zv);
	} else {
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

/* {{{ complex_operator_pow
 *
 * Backs the ZEND_POW case of complex_do_operation(): resolves the exponent (op2) into a (real,
 * imaginary) pair, then the base (op1) into a genuine zend_object* -- promoting it to a temporary
 * Complex first if it isn't already one -- since complex_calc_pow() (complex_power.c) needs a real
 * object for its general-case ln() call to read/cache the magnitude/phase computed properties.
 * Mirrors how complex_transcendental.c's log() promotes a scalar $base for the same reason.
 */
static zend_result complex_operator_pow(zval *result, zval *op1, zval *op2)
{
	double other_real, other_imag;
	if (complex_operand_to_parts(op2, &other_real, &other_imag) == FAILURE) {
		return FAILURE;
	}

	zval base_storage;
	bool base_is_temp = false;
	zend_object *base_obj;

	if (Z_TYPE_P(op1) == IS_OBJECT && instanceof_function(Z_OBJCE_P(op1), complex_ce_Complex)) {
		base_obj = Z_OBJ_P(op1);
	} else {
		double base_real, base_imag;
		if (complex_operand_to_parts(op1, &base_real, &base_imag) == FAILURE) {
			return FAILURE;
		}
		if (complex_create(&base_storage, base_real, base_imag) == FAILURE) {
			return FAILURE;
		}
		base_obj = Z_OBJ(base_storage);
		base_is_temp = true;
	}

	double out_real, out_imag;
	zend_result calc_result = complex_calc_pow(base_obj, other_real, other_imag, &out_real, &out_imag);

	if (calc_result == SUCCESS) {
		calc_result = complex_create(result, out_real, out_imag);
	}

	if (base_is_temp) {
		zval_ptr_dtor(&base_storage);
	}

	return calc_result;
}
/* }}} */

/* {{{ complex_do_operation
 *
 * The `do_operation` object handler backing Complex's operator overloads: +, -, *, /, ** for
 * Complex|int|float on either side, and ~ (conjugate) for the single Complex operand. Installed
 * on complex_object_handlers in complex_properties_minit() (complex_properties.c).
 *
 * Called by the engine whenever at least one operand of one of the above opcodes is an object
 * with a do_operation handler (op1 checked first, then op2 -- see
 * ZEND_TRY_BINARY_OBJECT_OPERATION()/ZEND_TRY_UNARY_OBJECT_OPERATION() in Zend/zend_operators.c).
 * For ZEND_BW_NOT (unary), op2 is always NULL and op1 is always the Complex being conjugated --
 * PHP has no dedicated opcode for unary +/-; the compiler lowers `-$z`/`+$z` to `$z * -1`/`$z * 1`
 * (see zend_compile_unary_pm() in Zend/zend_compile.c), so those already work correctly through
 * the ZEND_MUL case below without any special-casing here.
 *
 * Returning FAILURE without throwing (for an operand of any type other than Complex/int/float)
 * lets the engine fall through to its own standard "Unsupported operand types" TypeError.
 */
zend_result complex_do_operation(uint8_t opcode, zval *result, zval *op1, zval *op2)
{
	if (opcode == ZEND_BW_NOT) {
		double real, imag;
		complex_read_parts(Z_OBJ_P(op1), &real, &imag);
		return complex_create(result, real, -imag);
	}

	if (opcode == ZEND_POW) {
		return complex_operator_pow(result, op1, op2);
	}

	if (opcode != ZEND_ADD && opcode != ZEND_SUB && opcode != ZEND_MUL && opcode != ZEND_DIV) {
		return FAILURE;
	}

	double a_real, a_imag, b_real, b_imag;
	if (complex_operand_to_parts(op1, &a_real, &a_imag) == FAILURE) {
		return FAILURE;
	}
	if (complex_operand_to_parts(op2, &b_real, &b_imag) == FAILURE) {
		return FAILURE;
	}

	switch (opcode) {
		case ZEND_ADD:
			return complex_create(result, a_real + b_real, a_imag + b_imag);
		case ZEND_SUB:
			return complex_create(result, a_real - b_real, a_imag - b_imag);
		case ZEND_MUL:
			return complex_create(result, a_real * b_real - a_imag * b_imag, a_real * b_imag + a_imag * b_real);
		case ZEND_DIV: {
			if (b_real == 0.0 && b_imag == 0.0) {
				zend_throw_exception(math_ce_ArithmeticException, "Cannot divide by zero.", 0);
				return FAILURE;
			}
			double f = (b_real * b_real) + (b_imag * b_imag);
			return complex_create(result, (a_real * b_real + a_imag * b_imag) / f, (a_imag * b_real - a_real * b_imag) / f);
		}
		default:
			return FAILURE;
	}
}
/* }}} */
