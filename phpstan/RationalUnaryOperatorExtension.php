<?php

declare(strict_types=1);

namespace OceanMoon\Math\PHPStan;

use OceanMoon\Math\Rational;
use PHPStan\Type\ObjectType;
use PHPStan\Type\Type;
use PHPStan\Type\UnaryOperatorTypeSpecifyingExtension;

/**
 * Teaches PHPStan about Rational's unary operator overloading: + (identity) and - (negate) --
 * unlike Complex, Rational has no ~ (conjugate has no meaning for a rational number). Both are
 * backed by the same `do_operation` handler as the binary operators (see
 * RationalBinaryOperatorExtension) -- PHP has no dedicated opcode for unary +/-, so the compiler
 * lowers `-$r`/`+$r` to `$r * -1`/`$r * 1` (see ComplexUnaryOperatorExtension's doc comment for
 * the fuller explanation), but PHPStan works from the parsed AST, where these are distinct unary
 * nodes, so both need listing here regardless of how the VM later lowers them.
 *
 * Registered in phpstan.neon via the phpstan.broker.unaryOperatorTypeSpecifyingExtension service
 * tag.
 */
final class RationalUnaryOperatorExtension implements UnaryOperatorTypeSpecifyingExtension
{
    private const OPERATORS = ['+', '-'];

    public function isOperatorSupported(string $operatorSigil, Type $operand): bool
    {
        if (!in_array($operatorSigil, self::OPERATORS, true)) {
            return false;
        }

        return (new ObjectType(Rational::class))->isSuperTypeOf($operand)->yes();
    }

    public function specifyType(string $operatorSigil, Type $operand): Type
    {
        return new ObjectType(Rational::class);
    }
}
