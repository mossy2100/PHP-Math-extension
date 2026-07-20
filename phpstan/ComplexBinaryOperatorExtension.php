<?php

declare(strict_types=1);

namespace OceanMoon\Math\PHPStan;

use OceanMoon\Math\Complex;
use PHPStan\Type\FloatType;
use PHPStan\Type\IntegerType;
use PHPStan\Type\ObjectType;
use PHPStan\Type\OperatorTypeSpecifyingExtension;
use PHPStan\Type\Type;
use PHPStan\Type\UnionType;

/**
 * Teaches PHPStan about Complex's binary operator overloading (+, -, *, /, **), which only exists
 * at the C level (the `do_operation` object handler in Complex/complex_operators.c) and so is
 * otherwise entirely invisible to static analysis -- see docs/Complex.md for the runtime behavior
 * this mirrors. Same problem, and same fix, as PHPStan needing this for GMP/BCMath, which is what
 * this extension type is designed for; see
 * https://phpstan.org/developing-extensions/operator-type-specifying-extensions.
 *
 * Registered in phpstan.neon via the phpstan.broker.operatorTypeSpecifyingExtension service tag.
 */
final class ComplexBinaryOperatorExtension implements OperatorTypeSpecifyingExtension
{
    /**
     * The binary operators Complex overloads. Matches complex_do_operation()'s switch in
     * Complex/complex_operators.c, other than `~` (always unary) and `+`/`-` when used as unary
     * operators, all three of which PHPStan treats as distinct AST nodes from their binary
     * counterparts -- see ComplexUnaryOperatorExtension.
     */
    private const OPERATORS = ['+', '-', '*', '/', '**'];

    public function isOperatorSupported(string $operatorSigil, Type $leftSide, Type $rightSide): bool
    {
        if (!in_array($operatorSigil, self::OPERATORS, true)) {
            return false;
        }

        // At least one side must definitely be a Complex -- otherwise this is ordinary int|float
        // arithmetic PHPStan already handles fine on its own.
        $complexType = new ObjectType(Complex::class);
        if (!$complexType->isSuperTypeOf($leftSide)->yes() && !$complexType->isSuperTypeOf($rightSide)->yes()) {
            return false;
        }

        // Both sides must be Complex|int|float, matching what complex_operand_to_parts() actually
        // accepts at runtime -- anything else (e.g. Complex + string) should fall through to
        // PHPStan's own "invalid binary operation" handling instead of being claimed here.
        return $this->isComplexOrScalar($leftSide) && $this->isComplexOrScalar($rightSide);
    }

    public function specifyType(string $operatorSigil, Type $leftSide, Type $rightSide): Type
    {
        return new ObjectType(Complex::class);
    }

    private function isComplexOrScalar(Type $type): bool
    {
        $accepted = new UnionType([
            new ObjectType(Complex::class),
            new IntegerType(),
            new FloatType(),
        ]);

        return $accepted->isSuperTypeOf($type)->yes();
    }
}
