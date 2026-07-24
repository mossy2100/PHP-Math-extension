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

        // The `+` and `-` operators only accept Vector +/- Vector. No scalar form exists.
        if ($operatorSigil === '+' || $operatorSigil === '-') {
            return $leftIsVector && $rightIsVector;
        }

        // The `/` operator only accepts Vector / int|float.
        // Vector / Vector and int|float / Vector forms are unsupported.
        if ($operatorSigil === '/') {
            return $leftIsVector && $this->isScalar($rightSide);
        }

        // The `*` accepts:
        // 1. Vector * int|float
        // 2. int|float * Vector
        // 3. Vector * Matrix
        // Matrix * Vector is supported, but not here; see MatrixBinaryOperatorExtension.
        // Vector * Vector is unsupported, as there's no unambiguous meaning.
        if ($leftIsVector && $this->isScalar($rightSide)) {
            return true;
        }
        // Check for .
        if ($rightIsVector && $this->isScalar($leftSide)) {
            return true;
        }
        // Check for .
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
