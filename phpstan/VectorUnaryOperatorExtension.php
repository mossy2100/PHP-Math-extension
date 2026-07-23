<?php

declare(strict_types=1);

namespace OceanMoon\Math\PHPStan;

use OceanMoon\Math\Vector;
use PHPStan\Type\ObjectType;
use PHPStan\Type\Type;
use PHPStan\Type\UnaryOperatorTypeSpecifyingExtension;

/**
 * Teaches PHPStan about Vector's unary operator overloading: + (identity) and - (negate). Both are
 * backed by the same `do_operation` handler as the binary operators (see
 * VectorBinaryOperatorExtension) -- PHP has no dedicated opcode for unary +/-, so the compiler
 * lowers `-$v`/`+$v` to `$v * -1`/`$v * 1`, but PHPStan works from the parsed AST, where these are
 * distinct unary nodes, so both need listing here regardless of how the VM later lowers them.
 *
 * Registered in phpstan.neon via the phpstan.broker.unaryOperatorTypeSpecifyingExtension service
 * tag.
 */
final class VectorUnaryOperatorExtension implements UnaryOperatorTypeSpecifyingExtension
{
    private const OPERATORS = ['+', '-'];

    public function isOperatorSupported(string $operatorSigil, Type $operand): bool
    {
        if (!in_array($operatorSigil, self::OPERATORS, true)) {
            return false;
        }

        return (new ObjectType(Vector::class))->isSuperTypeOf($operand)->yes();
    }

    public function specifyType(string $operatorSigil, Type $operand): Type
    {
        return new ObjectType(Vector::class);
    }
}
