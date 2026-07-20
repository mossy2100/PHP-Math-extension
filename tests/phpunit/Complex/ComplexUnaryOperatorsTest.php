<?php

declare(strict_types=1);

namespace OceanMoon\Math\Tests\Complex;

use OceanMoon\Math\Complex;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\TestCase;

/**
 * Tests for Complex's unary operator overloads (+, -, ~), only available in the extension -- see
 * docs/Complex.md in this repo. Mirrors the structure of the package's own
 * ComplexUnaryArithmeticTest, whose neg()/conj() tests the - and ~ operators are equivalent to.
 */
#[CoversClass(Complex::class)]
class ComplexUnaryOperatorsTest extends TestCase
{
    #region Operator + (identity) tests.

    /**
     * Test that unary + returns an equal Complex with the same real and imaginary parts.
     */
    public function testUnaryPlus(): void
    {
        $z = new Complex(3, 4);
        $copy = +$z;

        $this->assertSame(3.0, $copy->real);
        $this->assertSame(4.0, $copy->imaginary);
    }

    /**
     * Test that unary + returns a new instance, not $z itself (immutability).
     */
    public function testUnaryPlusReturnsNewInstance(): void
    {
        $z = new Complex(3, 4);
        $copy = +$z;

        $this->assertNotSame($z, $copy);
    }

    /**
     * Test unary + on zero and on a purely imaginary number.
     */
    public function testUnaryPlusEdgeCases(): void
    {
        $zero = +new Complex(0, 0);
        $this->assertSame(0.0, $zero->real);
        $this->assertSame(0.0, $zero->imaginary);

        $imaginary = +new Complex(0, -5);
        $this->assertSame(0.0, $imaginary->real);
        $this->assertSame(-5.0, $imaginary->imaginary);
    }

    #endregion

    #region Operator - (negate) tests.

    /**
     * Test that unary - is equivalent to neg(): negates both parts.
     */
    public function testUnaryMinus(): void
    {
        $z = new Complex(3, 4);
        $negated = -$z;

        $this->assertSame(-3.0, $negated->real);
        $this->assertSame(-4.0, $negated->imaginary);
        $this->assertEquals($z->neg(), $negated);
    }

    /**
     * Test that unary - does not modify the original (immutability).
     */
    public function testUnaryMinusDoesNotMutate(): void
    {
        $z = new Complex(3, 4);

        $z2 = -$z;

        $this->assertSame(3.0, $z->real);
        $this->assertSame(4.0, $z->imaginary);
    }

    /**
     * Test unary - on zero is unchanged (no signed zero artifacts).
     */
    public function testUnaryMinusOnZero(): void
    {
        $result = -new Complex(0, 0);

        $this->assertSame(0.0, $result->real);
        $this->assertSame(0.0, $result->imaginary);
    }

    /**
     * Test double negation returns the original value.
     */
    public function testDoubleUnaryMinus(): void
    {
        $z = new Complex(3, 4);
        $result = -(-$z);

        $this->assertSame(3.0, $result->real);
        $this->assertSame(4.0, $result->imaginary);
    }

    #endregion

    #region Operator ~ (conjugate) tests.

    /**
     * Test that ~ is equivalent to conj(): negates the imaginary part only.
     */
    public function testConjugate(): void
    {
        $z = new Complex(3, 4);
        $result = ~$z;

        $this->assertSame(3.0, $result->real);
        $this->assertSame(-4.0, $result->imaginary);
        $this->assertEquals($z->conj(), $result);
    }

    /**
     * Test that ~ does not modify the original (immutability).
     */
    public function testConjugateDoesNotMutate(): void
    {
        $z = new Complex(3, 4);

        $z2 = ~$z;

        $this->assertSame(3.0, $z->real);
        $this->assertSame(4.0, $z->imaginary);
    }

    /**
     * Test conjugate of a real number (imaginary part 0) is unchanged.
     */
    public function testConjugateOfReal(): void
    {
        $result = ~new Complex(5, 0);

        $this->assertSame(5.0, $result->real);
        $this->assertSame(0.0, $result->imaginary);
    }

    /**
     * Test double conjugation returns the original value.
     */
    public function testDoubleConjugate(): void
    {
        $z = new Complex(3, 4);
        $result = ~(~$z);

        $this->assertSame(3.0, $result->real);
        $this->assertSame(4.0, $result->imaginary);
    }

    #endregion
}
