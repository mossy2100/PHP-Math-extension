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
        $r1 = new Rational(2, 3);
        $r2 = +$r1;

        $this->assertSame(2, $r2->numerator);
        $this->assertSame(3, $r2->denominator);
    }

    /**
     * Test that unary + returns a new instance, not $r1 itself (immutability).
     */
    public function testUnaryPlusReturnsNewInstance(): void
    {
        $r1 = new Rational(2, 3);
        $r2 = +$r1;

        $this->assertNotSame($r1, $r2);
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
        $r1 = new Rational(2, 3);

        $r2 = -$r1;

        $this->assertSame(2, $r1->numerator);
        $this->assertSame(3, $r1->denominator);
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
