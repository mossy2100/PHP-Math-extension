<?php

declare(strict_types=1);

namespace OceanMoon\Math\Tests\Rational;

use OceanMoon\Math\Rational;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\TestCase;

/**
 * Tests for Rational's unary operator overloads (+, -), only available in the extension -- see
 * docs/Rational.md in this repo. Mirrors the structure of the package's own
 * RationalUnaryArithmeticTest, whose neg() tests the - operator is equivalent to.
 */
#[CoversClass(Rational::class)]
class RationalUnaryOperatorsTest extends TestCase
{
    #region Operator + (identity) tests.

    /**
     * Test that unary + returns an equal Rational with the same numerator and denominator.
     */
    public function testUnaryPlus(): void
    {
        $r = new Rational(2, 3);
        $copy = +$r;

        $this->assertSame(2, $copy->numerator);
        $this->assertSame(3, $copy->denominator);
    }

    /**
     * Test that unary + returns a new instance, not $r itself (immutability).
     */
    public function testUnaryPlusReturnsNewInstance(): void
    {
        $r = new Rational(2, 3);
        $copy = +$r;

        $this->assertNotSame($r, $copy);
    }

    /**
     * Test unary + on zero and on a negative value.
     */
    public function testUnaryPlusEdgeCases(): void
    {
        $zero = +new Rational(0);
        $this->assertSame(0, $zero->numerator);
        $this->assertSame(1, $zero->denominator);

        $negative = +new Rational(-2, 3);
        $this->assertSame(-2, $negative->numerator);
        $this->assertSame(3, $negative->denominator);
    }

    #endregion

    #region Operator - (negate) tests.

    /**
     * Test that unary - is equivalent to neg(): negates the numerator.
     */
    public function testUnaryMinus(): void
    {
        $r = new Rational(2, 3);
        $negated = -$r;

        $this->assertSame(-2, $negated->numerator);
        $this->assertSame(3, $negated->denominator);
        $this->assertEquals($r->neg(), $negated);
    }

    /**
     * Test that unary - does not modify the original (immutability).
     */
    public function testUnaryMinusDoesNotMutate(): void
    {
        $r = new Rational(2, 3);

        $r2 = -$r;

        $this->assertSame(2, $r->numerator);
        $this->assertSame(3, $r->denominator);
    }

    /**
     * Test unary - on zero is unchanged.
     */
    public function testUnaryMinusOnZero(): void
    {
        $result = -new Rational(0);

        $this->assertSame(0, $result->numerator);
        $this->assertSame(1, $result->denominator);
    }

    /**
     * Test double negation returns the original value.
     */
    public function testDoubleUnaryMinus(): void
    {
        $r = new Rational(2, 3);
        $result = -(-$r);

        $this->assertSame(2, $result->numerator);
        $this->assertSame(3, $result->denominator);
    }

    #endregion
}
