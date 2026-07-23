<?php

declare(strict_types=1);

namespace OceanMoon\Math\PHPStan;

use OceanMoon\Math\Matrix;
use OceanMoon\Math\Vector;
use PHPStan\Type\FloatType;
use PHPStan\Type\IntegerType;
use PHPStan\Type\ObjectType;
use PHPStan\Type\OperatorTypeSpecifyingExtension;
use PHPStan\Type\Type;
use PHPStan\Type\UnionType;

/**
 * Teaches PHPStan about Matrix's binary operator overloading (+, -, *, /, **), which only exists at
 * the C level (the `do_operation` object handler in Matrix/matrix_operators.c) and so is otherwise
 * entirely invisible to static analysis -- see docs/Matrix.md for the runtime behavior this
 * mirrors. Same approach as ComplexBinaryOperatorExtension/RationalBinaryOperatorExtension.
 *
 * Unlike Rational, there's no float-widening rule here: every supported form results in a Matrix,
 * except `Matrix * Vector`, which results in a Vector (equivalent to mulVector()) -- specifyType()
 * only needs to special-case that one combination.
 *
 * Registered in phpstan.neon via the phpstan.broker.operatorTypeSpecifyingExtension service tag.
 */
final class MatrixBinaryOperatorExtension implements OperatorTypeSpecifyingExtension
{
    /**
     * The binary operators Matrix overloads. Matches matrix_do_operation()'s switch in
     * Matrix/matrix_operators.c, other than `+`/`-` when used as unary operators, which PHPStan
     * treats as distinct AST nodes from their binary counterparts -- see
     * MatrixUnaryOperatorExtension.
     */
    private const OPERATORS = ['+', '-', '*', '/', '**'];

    public function isOperatorSupported(string $operatorSigil, Type $leftSide, Type $rightSide): bool
    {
        if (!in_array($operatorSigil, self::OPERATORS, true)) {
            return false;
        }

        $matrixType = new ObjectType(Matrix::class);
        $leftIsMatrix = $matrixType->isSuperTypeOf($leftSide)->yes();
        $rightIsMatrix = $matrixType->isSuperTypeOf($rightSide)->yes();

        // + and - only accept Matrix + Matrix -- no scalar form exists.
        if ($operatorSigil === '+' || $operatorSigil === '-') {
            return $leftIsMatrix && $rightIsMatrix;
        }

        // / only accepts Matrix / int|float, with the Matrix on the left -- no Matrix / Matrix, no
        // int|float / Matrix.
        if ($operatorSigil === '/') {
            return $leftIsMatrix && $this->isScalar($rightSide);
        }

        // ** only accepts Matrix ** int, with the Matrix on the left -- no int ** Matrix, no
        // Matrix ** Matrix.
        if ($operatorSigil === '**') {
            return $leftIsMatrix && (new IntegerType())->isSuperTypeOf($rightSide)->yes();
        }

        // * accepts Matrix * int|float, int|float * Matrix (commutative), Matrix * Matrix, or
        // Matrix * Vector (Matrix on the left only -- Vector * Matrix is a distinct calculation
        // handled by VectorBinaryOperatorExtension instead).
        if ($leftIsMatrix && $this->isScalar($rightSide)) {
            return true;
        }
        if ($rightIsMatrix && $this->isScalar($leftSide)) {
            return true;
        }
        if ($leftIsMatrix && $rightIsMatrix) {
            return true;
        }
        return $leftIsMatrix && (new ObjectType(Vector::class))->isSuperTypeOf($rightSide)->yes();
    }

    public function specifyType(string $operatorSigil, Type $leftSide, Type $rightSide): Type
    {
        // Matrix * Vector is the one form that doesn't result in a Matrix -- equivalent to
        // mulVector(), it results in a Vector.
        if ($operatorSigil === '*' && (new ObjectType(Vector::class))->isSuperTypeOf($rightSide)->yes()) {
            return new ObjectType(Vector::class);
        }

        return new ObjectType(Matrix::class);
    }

    private function isScalar(Type $type): bool
    {
        $accepted = new UnionType([new IntegerType(), new FloatType()]);

        return $accepted->isSuperTypeOf($type)->yes();
    }
}
