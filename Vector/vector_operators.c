/*
 * vector_operators.c
 *
 * Operator overloading for OceanMoon\Math\Vector: +, -, *, / (plus unary - (negate), which PHP
 * lowers to `$v * -1` -- see vector_do_operation()'s doc comment). Mirrors docs/Vector.md in this
 * repo -- these operators only exist here in the extension; the userland package has no way to
 * overload operators, so it exposes the same behavior only as named methods (add()/sub()/mul()/
 * div()/neg()). No `**` (raising a vector to a power isn't a valid operation) and no comparison
 * operators (vectors have no natural sort order).
 *
 * Delegates entirely to the same "calc core" functions the named methods use (vector_calc_add()/
 * vector_calc_sub()/vector_calc_mul_scalar()/vector_calc_mul_matrix()/vector_calc_div(), all in
 * vector_arithmetic.c), so operator and method results/exceptions are guaranteed identical.
 *
 * Installed on vector_object_handlers in vector_properties_minit() (vector_properties.c),
 * alongside that table's `read_property` override -- Vector, like Complex, needs two different
 * handler overrides on the one custom handlers table, so there's no separate handlers table or
 * create_object function here.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "vector_internal.h"
#include "vector_arginfo.h"
#include "../Matrix/matrix_internal.h"

/* {{{ vector_operand_scalar
 *
 * Resolves a zval operand to a double if it's an int or float, for the scalar forms of `*`/`/`.
 * Returns FAILURE without an exception for any other type, so the caller can return FAILURE from
 * the do_operation handler in turn, letting the engine fall through to its own standard
 * "Unsupported operand types" TypeError.
 */
static zend_result vector_operand_scalar(zval *zv, double *out)
{
	if (Z_TYPE_P(zv) == IS_DOUBLE) {
		*out = Z_DVAL_P(zv);
		return SUCCESS;
	}
	if (Z_TYPE_P(zv) == IS_LONG) {
		*out = (double) Z_LVAL_P(zv);
		return SUCCESS;
	}
	return FAILURE;
}
/* }}} */

/* {{{ vector_do_operation
 *
 * The `do_operation` object handler backing Vector's operator overloads: +, -, *, / -- see
 * docs/Vector.md for the exact supported forms of each. Unary - needs no special-casing here: PHP
 * has no dedicated opcode for it, and the compiler lowers `-$v` to `$v * -1` (unary + similarly
 * lowers to `$v * 1`), so both are already covered by the ZEND_MUL case below.
 *
 * Called by the engine whenever at least one operand of one of the above opcodes is an object with
 * a do_operation handler (op1 checked first, then op2). Returning FAILURE (without throwing) for
 * any unsupported operand combination -- including Vector + int, Vector * Vector, int / Vector,
 * Vector / Vector, and Vector / Matrix, all deliberately unsupported per docs/Vector.md -- lets the
 * engine fall through to its own standard "Unsupported operand types" TypeError, or try the other
 * operand's handler if it has one (e.g. `Matrix * Vector`, handled by matrix_do_operation instead).
 */
zend_result vector_do_operation(uint8_t opcode, zval *result, zval *op1, zval *op2)
{
	bool op1_is_vector = Z_TYPE_P(op1) == IS_OBJECT && instanceof_function(Z_OBJCE_P(op1), vector_ce_Vector);
	bool op2_is_vector = Z_TYPE_P(op2) == IS_OBJECT && instanceof_function(Z_OBJCE_P(op2), vector_ce_Vector);

	switch (opcode) {
		case ZEND_ADD:
			if (op1_is_vector && op2_is_vector) {
				return vector_calc_add(Z_OBJ_P(op1), Z_OBJ_P(op2), result);
			}
			return FAILURE;

		case ZEND_SUB:
			if (op1_is_vector && op2_is_vector) {
				return vector_calc_sub(Z_OBJ_P(op1), Z_OBJ_P(op2), result);
			}
			return FAILURE;

		case ZEND_MUL: {
			double scalar;
			if (op1_is_vector && vector_operand_scalar(op2, &scalar) == SUCCESS) {
				return vector_calc_mul_scalar(Z_OBJ_P(op1), scalar, result);
			}
			if (op2_is_vector && vector_operand_scalar(op1, &scalar) == SUCCESS) {
				return vector_calc_mul_scalar(Z_OBJ_P(op2), scalar, result);
			}
			if (op1_is_vector && Z_TYPE_P(op2) == IS_OBJECT && instanceof_function(Z_OBJCE_P(op2), matrix_ce_Matrix)) {
				return vector_calc_mul_matrix(Z_OBJ_P(op1), Z_OBJ_P(op2), result);
			}
			return FAILURE;
		}

		case ZEND_DIV: {
			double scalar;
			if (op1_is_vector && vector_operand_scalar(op2, &scalar) == SUCCESS) {
				return vector_calc_div(Z_OBJ_P(op1), scalar, result);
			}
			return FAILURE;
		}

		default:
			return FAILURE;
	}
}
/* }}} */
