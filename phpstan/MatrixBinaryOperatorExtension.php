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

        // The `+` and `-` operators only accept Matrix +/- Matrix. No scalar form exists.
        if ($operatorSigil === '+' || $operatorSigil === '-') {
            return $leftIsMatrix && $rightIsMatrix;
        }

        // The `/` operator only accepts Matrix / int|float.
        // Matrix / Matrix and int|float / Matrix forms are unsupported.
        if ($operatorSigil === '/') {
            return $leftIsMatrix && $this->isScalar($rightSide);
        }

        // The `**` operator only accepts Matrix ** int.
        // int ** Matrix and Matrix ** Matrix forms are unsupported.
        if ($operatorSigil === '**') {
            return $leftIsMatrix && (new IntegerType())->isSuperTypeOf($rightSide)->yes();
        }

        // The `*` operator accepts:
        // 1. Matrix * int|float
        // 2. int|float * Matrix
        // 3. Matrix * Matrix
        // 4. Matrix * Vector
        // Vector * Matrix is supported, but not here; see VectorBinaryOperatorExtension.
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
