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
 * Teaches PHPStan about Vector's binary operator overloading (+, -, *, /), which only exists at
 * the C level (the `do_operation` object handler in Vector/vector_operators.c) and so is otherwise
 * entirely invisible to static analysis -- see docs/Vector.md for the runtime behavior this
 * mirrors. Same approach as ComplexBinaryOperatorExtension/RationalBinaryOperatorExtension.
 *
 * Unlike Rational, every supported form here always results in a Vector -- there's no
 * float-widening rule -- so specifyType() can return a constant like ComplexBinaryOperatorExtension
 * does, rather than computing the result type from the operand types.
 *
 * Registered in phpstan.neon via the phpstan.broker.operatorTypeSpecifyingExtension service tag.
 */
final class VectorBinaryOperatorExtension implements OperatorTypeSpecifyingExtension
{
    /**
     * The binary operators Vector overloads. Matches vector_do_operation()'s switch in
     * Vector/vector_operators.c, other than `+`/`-` when used as unary operators, which PHPStan
     * treats as distinct AST nodes from their binary counterparts -- see
     * VectorUnaryOperatorExtension. No `**`: raising a vector to a power isn't a valid operation.
     */
    private const OPERATORS = ['+', '-', '*', '/'];

    public function isOperatorSupported(string $operatorSigil, Type $leftSide, Type $rightSide): bool
    {
        if (!in_array($operatorSigil, self::OPERATORS, true)) {
            return false;
        }

        $vectorType = new ObjectType(Vector::class);
        $leftIsVector = $vectorType->isSuperTypeOf($leftSide)->yes();
        $rightIsVector = $vectorType->isSuperTypeOf($rightSide)->yes();

        // + and - only accept Vector + Vector -- no scalar form exists.
        if ($operatorSigil === '+' || $operatorSigil === '-') {
            return $leftIsVector && $rightIsVector;
        }

        // / only accepts Vector / int|float, with the Vector on the left -- matching
        // vector_do_operation()'s ZEND_DIV case exactly (no scalar/Vector, no Vector/Vector, no
        // Vector/Matrix).
        if ($operatorSigil === '/') {
            return $leftIsVector && $this->isScalar($rightSide);
        }

        // * accepts Vector * int|float, int|float * Vector (commutative), or Vector * Matrix
        // (Vector on the left only -- Matrix * Vector is a distinct calculation handled by
        // MatrixBinaryOperatorExtension instead). No Vector * Vector.
        if ($leftIsVector && $this->isScalar($rightSide)) {
            return true;
        }
        if ($rightIsVector && $this->isScalar($leftSide)) {
            return true;
        }
        return $leftIsVector && (new ObjectType(Matrix::class))->isSuperTypeOf($rightSide)->yes();
    }

    public function specifyType(string $operatorSigil, Type $leftSide, Type $rightSide): Type
    {
        return new ObjectType(Vector::class);
    }

    private function isScalar(Type $type): bool
    {
        $accepted = new UnionType([new IntegerType(), new FloatType()]);

        return $accepted->isSuperTypeOf($type)->yes();
    }
}
