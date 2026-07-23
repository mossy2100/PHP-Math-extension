<?php

declare(strict_types=1);

namespace OceanMoon\Math\PHPStan;

use OceanMoon\Math\Matrix;
use PHPStan\Type\ObjectType;
use PHPStan\Type\Type;
use PHPStan\Type\UnaryOperatorTypeSpecifyingExtension;

/**
 * Teaches PHPStan about Matrix's unary operator overloading: + (identity) and - (negate). Both are
 * backed by the same `do_operation` handler as the binary operators (see
 * MatrixBinaryOperatorExtension) -- PHP has no dedicated opcode for unary +/-, so the compiler
 * lowers `-$A`/`+$A` to `$A * -1`/`$A * 1`, but PHPStan works from the parsed AST, where these are
 * distinct unary nodes, so both need listing here regardless of how the VM later lowers them.
 *
 * Registered in phpstan.neon via the phpstan.broker.unaryOperatorTypeSpecifyingExtension service
 * tag.
 */
final class MatrixUnaryOperatorExtension implements UnaryOperatorTypeSpecifyingExtension
{
    private const OPERATORS = ['+', '-'];

    public function isOperatorSupported(string $operatorSigil, Type $operand): bool
    {
        if (!in_array($operatorSigil, self::OPERATORS, true)) {
            return false;
        }

        return (new ObjectType(Matrix::class))->isSuperTypeOf($operand)->yes();
    }

    public function specifyType(string $operatorSigil, Type $operand): Type
    {
        return new ObjectType(Matrix::class);
    }
}
