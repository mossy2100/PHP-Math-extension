<?php

declare(strict_types=1);

namespace OceanMoon\Math\PHPStan;

use OceanMoon\Math\Complex;
use PHPStan\Type\ObjectType;
use PHPStan\Type\Type;
use PHPStan\Type\UnaryOperatorTypeSpecifyingExtension;

/**
 * Teaches PHPStan about Complex's unary operator overloading: + (identity), - (negate), and ~
 * (conjugate). All three are backed by the same `do_operation` handler as the binary operators
 * (see ComplexBinaryOperatorExtension) -- the Zend VM itself has no dedicated opcode for unary +/-, and
 * lowers `+$z`/`-$z` to `$z * 1`/`$z * -1` at compile time (see complex_do_operation()'s doc
 * comment in Complex/complex_operators.c), but that's a runtime-compilation detail PHPStan never
 * sees: PHPStan works from the original parsed AST, where `+$z` and `-$z` are distinct unary
 * nodes, so both need listing here regardless of how the VM later lowers them.
 *
 * Registered in phpstan.neon via the phpstan.broker.unaryOperatorTypeSpecifyingExtension service
 * tag.
 */
final class ComplexUnaryOperatorExtension implements UnaryOperatorTypeSpecifyingExtension
{
    private const OPERATORS = ['+', '-', '~'];

    public function isOperatorSupported(string $operatorSigil, Type $operand): bool
    {
        if (!in_array($operatorSigil, self::OPERATORS, true)) {
            return false;
        }

        return (new ObjectType(Complex::class))->isSuperTypeOf($operand)->yes();
    }

    public function specifyType(string $operatorSigil, Type $operand): Type
    {
        return new ObjectType(Complex::class);
    }
}
