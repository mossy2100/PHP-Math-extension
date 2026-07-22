<?php

declare(strict_types=1);

namespace OceanMoon\Math\PHPStan;

use OceanMoon\Math\Rational;
use PHPStan\Type\BooleanType;
use PHPStan\Type\FloatType;
use PHPStan\Type\IntegerType;
use PHPStan\Type\ObjectType;
use PHPStan\Type\OperatorTypeSpecifyingExtension;
use PHPStan\Type\Type;
use PHPStan\Type\UnionType;

/**
 * Teaches PHPStan about Rational's comparison operator overloading (<, <=, >, >=, <=>, ==, !=),
 * which only exists at the C level (the `compare` object handler in
 * Rational/rational_operators.c) and so is otherwise entirely invisible to static analysis. `==`/
 * `!=` are covered here too, since PHP wires them to the same `compare` handler as an unavoidable
 * side effect of `<=>` existing -- see docs/Rational.md. Never applies to ===/!==, which always
 * mean reference identity for objects regardless of any compare handler.
 *
 * Registered in phpstan.neon via the phpstan.broker.operatorTypeSpecifyingExtension service tag,
 * same as the arithmetic extensions -- PHPStan doesn't have a separate extension point for the
 * `compare` object handler specifically; comparison sigils go through the same interface.
 */
final class RationalComparisonOperatorExtension implements OperatorTypeSpecifyingExtension
{
    private const OPERATORS = ['<=>', '<', '<=', '>', '>=', '==', '!='];

    public function isOperatorSupported(string $operatorSigil, Type $leftSide, Type $rightSide): bool
    {
        if (!in_array($operatorSigil, self::OPERATORS, true)) {
            return false;
        }

        $rationalType = new ObjectType(Rational::class);

        // At least one side must definitely be a Rational -- otherwise this is an ordinary
        // comparison PHPStan already handles fine on its own.
        if (!$rationalType->isSuperTypeOf($leftSide)->yes() && !$rationalType->isSuperTypeOf($rightSide)->yes()) {
            return false;
        }

        // Both sides must be Rational|int|float, matching what rational_calc_compare() actually
        // accepts at runtime (a string/array/etc. operand throws InvalidArgumentException instead
        // of comparing, so that case should fall through to PHPStan's own handling).
        return $this->isRationalOrScalar($leftSide) && $this->isRationalOrScalar($rightSide);
    }

    public function specifyType(string $operatorSigil, Type $leftSide, Type $rightSide): Type
    {
        return $operatorSigil === '<=>' ? new IntegerType() : new BooleanType();
    }

    private function isRationalOrScalar(Type $type): bool
    {
        $accepted = new UnionType([
            new ObjectType(Rational::class),
            new IntegerType(),
            new FloatType(),
        ]);

        return $accepted->isSuperTypeOf($type)->yes();
    }
}
