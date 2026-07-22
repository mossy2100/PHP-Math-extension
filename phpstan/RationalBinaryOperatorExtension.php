<?php

declare(strict_types=1);

namespace OceanMoon\Math\PHPStan;

use OceanMoon\Math\Rational;
use PHPStan\Type\FloatType;
use PHPStan\Type\IntegerType;
use PHPStan\Type\ObjectType;
use PHPStan\Type\OperatorTypeSpecifyingExtension;
use PHPStan\Type\Type;
use PHPStan\Type\UnionType;

/**
 * Teaches PHPStan about Rational's binary operator overloading (+, -, *, /, **), which only
 * exists at the C level (the `do_operation` object handler in
 * Rational/rational_operators.c) and so is otherwise entirely invisible to static analysis -- see
 * docs/Rational.md for the runtime behavior this mirrors. Same approach as
 * ComplexBinaryOperatorExtension.
 *
 * Unlike Complex, the result type here isn't always Rational: per docs/Rational.md's "Numeric
 * type widening" section, a genuine float operand widens the whole operation to float, so
 * specifyType() must compute the result type from the operand types rather than returning a
 * constant.
 *
 * Registered in phpstan.neon via the phpstan.broker.operatorTypeSpecifyingExtension service tag.
 */
final class RationalBinaryOperatorExtension implements OperatorTypeSpecifyingExtension
{
    /**
     * The binary operators Rational overloads. Matches rational_do_operation()'s switch in
     * Rational/rational_operators.c, other than `+`/`-` when used as unary operators, which
     * PHPStan treats as distinct AST nodes from their binary counterparts -- see
     * RationalUnaryOperatorExtension.
     */
    private const OPERATORS = ['+', '-', '*', '/', '**'];

    public function isOperatorSupported(string $operatorSigil, Type $leftSide, Type $rightSide): bool
    {
        if (!in_array($operatorSigil, self::OPERATORS, true)) {
            return false;
        }

        $rationalType = new ObjectType(Rational::class);
        $leftIsRational = $rationalType->isSuperTypeOf($leftSide)->yes();
        $rightIsRational = $rationalType->isSuperTypeOf($rightSide)->yes();

        if ($operatorSigil === '**') {
            // Three forms are supported: Rational ** int (exact), and Rational ** float /
            // float ** Rational (both widen to float). Rational ** Rational and int ** Rational
            // have no exact result and no float operand to trigger widening, so they're
            // unsupported -- see rational_do_pow()'s doc comment in rational_operators.c and
            // docs/Rational.md's "** (power)" section for the full reasoning. Anything else
            // should fall through to PHPStan's own "invalid binary operation" handling, matching
            // the runtime TypeError.
            if ($leftIsRational && (new IntegerType())->isSuperTypeOf($rightSide)->yes()) {
                return true;
            }
            if ($leftIsRational && (new FloatType())->isSuperTypeOf($rightSide)->yes()) {
                return true;
            }
            return $rightIsRational && (new FloatType())->isSuperTypeOf($leftSide)->yes();
        }

        // At least one side must definitely be a Rational -- otherwise this is ordinary
        // int|float arithmetic PHPStan already handles fine on its own.
        if (!$leftIsRational && !$rightIsRational) {
            return false;
        }

        // Both sides must be Rational|int|float, matching what rational_operand_to_parts()/
        // rational_operand_to_double() actually accept at runtime.
        return $this->isRationalOrScalar($leftSide) && $this->isRationalOrScalar($rightSide);
    }

    public function specifyType(string $operatorSigil, Type $leftSide, Type $rightSide): Type
    {
        $rationalType = new ObjectType(Rational::class);

        if ($operatorSigil === '**') {
            // Only the exact `Rational ** int` form stays Rational; both float-involving forms
            // (already the only other ones isOperatorSupported() allows) widen to float.
            if ($rationalType->isSuperTypeOf($leftSide)->yes() && (new IntegerType())->isSuperTypeOf($rightSide)->yes()) {
                return $rationalType;
            }
            return new FloatType();
        }

        if ($this->isFloat($leftSide) || $this->isFloat($rightSide)) {
            return new FloatType();
        }

        return $rationalType;
    }

    private function isFloat(Type $type): bool
    {
        return (new FloatType())->isSuperTypeOf($type)->yes();
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
