<?php

declare(strict_types=1);

namespace OceanMoon\Math\Tests\Complex;

use OceanMoon\Core\Exceptions\ArithmeticException;
use OceanMoon\Math\Complex;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\TestCase;

/**
 * Tests for Complex's binary operator overloads (+, -, *, /, **), only available in the
 * extension -- see docs/Complex.md in this repo. Each operator accepts a Complex or int|float
 * operand on either side and is equivalent to calling the same-named method, throwing the same
 * exceptions under the same conditions -- mirrors the structure of the package's own
 * ComplexBinaryArithmeticTest/ComplexPowerTest, whose add()/sub()/mul()/div()/pow() these
 * operators are equivalent to.
 */
#[CoversClass(Complex::class)]
class ComplexBinaryOperatorsTest extends TestCase
{
    #region Operator + (add) tests.

    /**
     * Test Complex + Complex.
     */
    public function testAddComplexPlusComplex(): void
    {
        $z1 = new Complex(3, 4);
        $z2 = new Complex(1, 2);
        $result = $z1 + $z2;

        $this->assertSame(4.0, $result->real);
        $this->assertSame(6.0, $result->imaginary);
        $this->assertEquals($z1->add($z2), $result);
    }

    /**
     * Test Complex + int|float.
     */
    public function testAddComplexPlusScalar(): void
    {
        $z = new Complex(3, 4);

        $result = $z + 2;
        $this->assertSame(5.0, $result->real);
        $this->assertSame(4.0, $result->imaginary);

        $result2 = $z + 2.5;
        $this->assertSame(5.5, $result2->real);
        $this->assertSame(4.0, $result2->imaginary);
    }

    /**
     * Test int|float + Complex gives the same result as Complex + int|float (commutative).
     */
    public function testAddScalarPlusComplex(): void
    {
        $z = new Complex(3, 4);

        $this->assertEquals($z + 2, 2 + $z);
    }

    /**
     * Test + does not modify either operand (immutability).
     */
    public function testAddDoesNotMutate(): void
    {
        $z1 = new Complex(3, 4);
        $z2 = new Complex(1, 2);

        $z3 = $z1 + $z2;

        $this->assertSame(3.0, $z1->real);
        $this->assertSame(4.0, $z1->imaginary);
        $this->assertSame(1.0, $z2->real);
        $this->assertSame(2.0, $z2->imaginary);
    }

    #endregion

    #region Operator - (subtract) tests.

    /**
     * Test Complex - Complex.
     */
    public function testSubComplexMinusComplex(): void
    {
        $z1 = new Complex(5, 7);
        $z2 = new Complex(2, 3);
        $result = $z1 - $z2;

        $this->assertSame(3.0, $result->real);
        $this->assertSame(4.0, $result->imaginary);
        $this->assertEquals($z1->sub($z2), $result);
    }

    /**
     * Test Complex - int|float.
     */
    public function testSubComplexMinusScalar(): void
    {
        $z = new Complex(5, 7);
        $result = $z - 2;

        $this->assertSame(3.0, $result->real);
        $this->assertSame(7.0, $result->imaginary);
    }

    /**
     * Test int|float - Complex: not commutative with Complex - int|float, the scalar is promoted
     * to a Complex with a zero imaginary part, then subtracted normally.
     */
    public function testSubScalarMinusComplex(): void
    {
        $z = new Complex(5, 7);
        $result = 2 - $z;

        $this->assertSame(-3.0, $result->real);
        $this->assertSame(-7.0, $result->imaginary);
    }

    /**
     * Test - does not modify either operand (immutability).
     */
    public function testSubDoesNotMutate(): void
    {
        $z1 = new Complex(5, 7);
        $z2 = new Complex(2, 3);

        $z3 = $z1 - $z2;

        $this->assertSame(5.0, $z1->real);
        $this->assertSame(7.0, $z1->imaginary);
        $this->assertSame(2.0, $z2->real);
        $this->assertSame(3.0, $z2->imaginary);
    }

    #endregion

    #region Operator * (multiply) tests.

    /**
     * Test Complex * Complex.
     */
    public function testMulComplexTimesComplex(): void
    {
        // (1 + 2i)(3 + 4i) = 3 + 4i + 6i + 8i² = -5 + 10i
        $z1 = new Complex(1, 2);
        $z2 = new Complex(3, 4);
        $result = $z1 * $z2;

        $this->assertSame(-5.0, $result->real);
        $this->assertSame(10.0, $result->imaginary);
        $this->assertEquals($z1->mul($z2), $result);
    }

    /**
     * Test Complex * int|float.
     */
    public function testMulComplexTimesScalar(): void
    {
        $z = new Complex(1, 2);
        $result = $z * 2;

        $this->assertSame(2.0, $result->real);
        $this->assertSame(4.0, $result->imaginary);
    }

    /**
     * Test int|float * Complex gives the same result as Complex * int|float (commutative).
     */
    public function testMulScalarTimesComplex(): void
    {
        $z = new Complex(1, 2);

        $this->assertEquals($z * 2, 2 * $z);
    }

    /**
     * Test * does not modify either operand (immutability).
     */
    public function testMulDoesNotMutate(): void
    {
        $z1 = new Complex(1, 2);
        $z2 = new Complex(3, 4);

        $z3 = $z1 * $z2;

        $this->assertSame(1.0, $z1->real);
        $this->assertSame(2.0, $z1->imaginary);
        $this->assertSame(3.0, $z2->real);
        $this->assertSame(4.0, $z2->imaginary);
    }

    #endregion

    #region Operator / (divide) tests.

    /**
     * Test Complex / Complex.
     */
    public function testDivComplexOverComplex(): void
    {
        // (6 + 8i) / (3 + 4i) = 2 (the imaginary parts cancel here)
        $z1 = new Complex(6, 8);
        $z2 = new Complex(3, 4);
        $result = $z1 / $z2;

        $this->assertEqualsWithDelta(2.0, $result->real, EPSILON);
        $this->assertEqualsWithDelta(0.0, $result->imaginary, EPSILON);
        $this->assertEqualsWithDelta($z1->div($z2)->real, $result->real, EPSILON);
        $this->assertEqualsWithDelta($z1->div($z2)->imaginary, $result->imaginary, EPSILON);
    }

    /**
     * Test Complex / int|float.
     */
    public function testDivComplexOverScalar(): void
    {
        $z = new Complex(6, 8);
        $result = $z / 2;

        $this->assertSame(3.0, $result->real);
        $this->assertSame(4.0, $result->imaginary);
    }

    /**
     * Test int|float / Complex: not commutative with Complex / int|float, the scalar is promoted
     * to a Complex with a zero imaginary part, then divided normally -- this also gives a way to
     * compute a multiplicative inverse (see "Inverting" tests below).
     */
    public function testDivScalarOverComplex(): void
    {
        $z = new Complex(3, 4);
        $result = 1 / $z;

        $this->assertSame(0.12, $result->real);
        $this->assertSame(-0.16, $result->imaginary);
    }

    /**
     * Test division by zero throws ArithmeticException, matching div().
     */
    public function testDivByZeroThrows(): void
    {
        $z = new Complex(3, 4);

        $this->expectException(ArithmeticException::class);
        $z / 0; // @phpstan-ignore binaryOp.invalid
    }

    /**
     * Test division by complex zero throws ArithmeticException, matching div().
     */
    public function testDivByComplexZeroThrows(): void
    {
        $z = new Complex(3, 4);

        $this->expectException(ArithmeticException::class);
        $z / new Complex(0, 0);
    }

    /**
     * Test / does not modify either operand (immutability).
     */
    public function testDivDoesNotMutate(): void
    {
        $z1 = new Complex(6, 8);
        $z2 = new Complex(3, 4);

        $z1 / $z2;

        $this->assertSame(6.0, $z1->real);
        $this->assertSame(8.0, $z1->imaginary);
        $this->assertSame(3.0, $z2->real);
        $this->assertSame(4.0, $z2->imaginary);
    }

    #endregion

    #region Operator ** (power) tests.

    /**
     * Test Complex ** Complex.
     */
    public function testPowComplexToComplex(): void
    {
        $z = new Complex(3, 4);
        $exponent = new Complex(2, 0);
        $result = $z ** $exponent;

        $this->assertEqualsWithDelta(-7.0, $result->real, EPSILON);
        $this->assertEqualsWithDelta(24.0, $result->imaginary, EPSILON);
        $this->assertEqualsWithDelta($z->pow($exponent)->real, $result->real, EPSILON);
        $this->assertEqualsWithDelta($z->pow($exponent)->imaginary, $result->imaginary, EPSILON);
    }

    /**
     * Test Complex ** int|float.
     */
    public function testPowComplexToScalar(): void
    {
        // (3 + 4i)² = 9 + 24i + 16i² = -7 + 24i
        $z = new Complex(3, 4);
        $result = $z ** 2;

        $this->assertEqualsWithDelta(-7.0, $result->real, EPSILON);
        $this->assertEqualsWithDelta(24.0, $result->imaginary, EPSILON);
    }

    /**
     * Test int|float ** Complex: not commutative in general with Complex ** int|float, the scalar
     * is promoted to a Complex with a zero imaginary part, then raised to the (possibly complex)
     * power on the right.
     */
    public function testPowScalarToComplex(): void
    {
        $z = new Complex(3, 4);
        $result = 2 ** $z;

        $this->assertEqualsWithDelta(-7.4614966146886, $result->real, EPSILON);
        $this->assertEqualsWithDelta(2.8854927255134, $result->imaginary, EPSILON);
    }

    /**
     * Test ** does not modify either operand (immutability).
     */
    public function testPowDoesNotMutate(): void
    {
        $z = new Complex(3, 4);

        $z2 = $z ** 2;

        $this->assertSame(3.0, $z->real);
        $this->assertSame(4.0, $z->imaginary);
    }

    #endregion

    #region Inverting with 1 / z or z ** -1 tests.

    /**
     * Test that 1 / $z, $z ** -1, and $z->inv() all give the same multiplicative inverse.
     */
    public function testInvertingEquivalence(): void
    {
        $z = new Complex(3, 4);

        $inv = $z->inv();
        $viaDivision = 1 / $z;
        $viaPower = $z ** -1;

        $this->assertSame(0.12, $inv->real);
        $this->assertSame(-0.16, $inv->imaginary);
        $this->assertEquals($inv, $viaDivision);
        $this->assertEquals($inv, $viaPower);
    }

    /**
     * Test that both inverting forms throw ArithmeticException for a zero operand, matching inv().
     */
    public function testInvertingZeroThrows(): void
    {
        $zero = new Complex(0, 0);

        $this->expectException(ArithmeticException::class);
        1 / $zero;
    }

    #endregion
}
