/*
 * matrix_operators.c
 *
 * Operator overloading for OceanMoon\Math\Matrix: +, -, *, /, ** (plus unary - (negate), which PHP
 * lowers to `$A * -1` -- see matrix_do_operation()'s doc comment). Mirrors docs/Matrix.md in this
 * repo -- these operators only exist here in the extension; the userland package has no way to
 * overload operators, so it exposes the same behavior only as named methods (add()/sub()/mul()/
 * div()/pow()/neg()). No comparison operators (matrices have no natural sort order).
 *
 * Delegates entirely to the same "calc core" functions the named methods use (matrix_calc_add()/
 * matrix_calc_sub()/matrix_calc_mul_scalar()/matrix_calc_mul_matrix()/matrix_calc_mul_vector()/
 * matrix_calc_div_scalar()/matrix_calc_pow(), spread across matrix_arithmetic.c/
 * matrix_linear_algebra.c/matrix_power.c), so operator and method results/exceptions are
 * guaranteed identical.
 *
 * Installed on matrix_object_handlers in matrix_minit() (matrix.c), alongside that table's
 * `clone_obj` override -- Matrix, like Vector, needs two different handler overrides on the one
 * custom handlers table.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "matrix_internal.h"
#include "matrix_arginfo.h"

/* {{{ matrix_operand_scalar
 *
 * Resolves a zval operand to a double if it's an int or float, for the scalar forms of `*`/`/`.
 * Returns FAILURE without an exception for any other type, so the caller can return FAILURE from
 * the do_operation handler in turn, letting the engine fall through to its own standard
 * "Unsupported operand types" TypeError.
 */
static zend_result matrix_operand_scalar(zval *zv, double *out)
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

/* {{{ matrix_do_operation
 *
 * The `do_operation` object handler backing Matrix's operator overloads: +, -, *, /, ** -- see
 * docs/Matrix.md for the exact supported forms of each. Unary - needs no special-casing here: PHP
 * has no dedicated opcode for it, and the compiler lowers `-$A` to `$A * -1` (unary + similarly
 * lowers to `$A * 1`), so both are already covered by the ZEND_MUL case below.
 *
 * Called by the engine whenever at least one operand of one of the above opcodes is an object with
 * a do_operation handler (op1 checked first, then op2). Returning FAILURE (without throwing) for
 * any unsupported operand combination -- including Matrix + int, Matrix * Matrix with incompatible
 * dimensions falling through to a thrown exception instead, Matrix / Matrix, int / Matrix, and
 * int ** Matrix, all deliberately unsupported per docs/Matrix.md -- lets the engine fall through to
 * its own standard "Unsupported operand types" TypeError, or try the other operand's handler if it
 * has one (e.g. `Vector * Matrix`, handled by vector_do_operation instead, since Vector is always
 * op1 in that expression and so is tried first).
 */
zend_result matrix_do_operation(uint8_t opcode, zval *result, zval *op1, zval *op2)
{
	bool op1_is_matrix = Z_TYPE_P(op1) == IS_OBJECT && instanceof_function(Z_OBJCE_P(op1), matrix_ce_Matrix);
	bool op2_is_matrix = Z_TYPE_P(op2) == IS_OBJECT && instanceof_function(Z_OBJCE_P(op2), matrix_ce_Matrix);

	switch (opcode) {
		case ZEND_ADD:
			if (op1_is_matrix && op2_is_matrix) {
				return matrix_calc_add(Z_OBJ_P(op1), Z_OBJ_P(op2), result);
			}
			return FAILURE;

		case ZEND_SUB:
			if (op1_is_matrix && op2_is_matrix) {
				return matrix_calc_sub(Z_OBJ_P(op1), Z_OBJ_P(op2), result);
			}
			return FAILURE;

		case ZEND_MUL: {
			double scalar;
			if (op1_is_matrix && matrix_operand_scalar(op2, &scalar) == SUCCESS) {
				return matrix_calc_mul_scalar(Z_OBJ_P(op1), scalar, result);
			}
			if (op2_is_matrix && matrix_operand_scalar(op1, &scalar) == SUCCESS) {
				return matrix_calc_mul_scalar(Z_OBJ_P(op2), scalar, result);
			}
			if (op1_is_matrix && op2_is_matrix) {
				return matrix_calc_mul_matrix(Z_OBJ_P(op1), Z_OBJ_P(op2), result);
			}
			if (op1_is_matrix && Z_TYPE_P(op2) == IS_OBJECT && instanceof_function(Z_OBJCE_P(op2), vector_ce_Vector)) {
				return matrix_calc_mul_vector(Z_OBJ_P(op1), Z_OBJ_P(op2), result);
			}
			return FAILURE;
		}

		case ZEND_DIV: {
			double scalar;
			if (op1_is_matrix && matrix_operand_scalar(op2, &scalar) == SUCCESS) {
				return matrix_calc_div_scalar(Z_OBJ_P(op1), scalar, result);
			}
			return FAILURE;
		}

		case ZEND_POW:
			if (op1_is_matrix && Z_TYPE_P(op2) == IS_LONG) {
				return matrix_calc_pow(Z_OBJ_P(op1), Z_LVAL_P(op2), result);
			}
			return FAILURE;

		default:
			return FAILURE;
	}
}
/* }}} */
