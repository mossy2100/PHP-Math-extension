<?php

declare(strict_types=1);

namespace OceanMoon\Math\Tests\Rational;

use OceanMoon\Core\Exceptions\ArithmeticException;
use OceanMoon\Math\Rational;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\TestCase;
use TypeError;

/**
 * Tests for Rational's binary arithmetic operator overloads (+, -, *, /, **), only available in
 * the extension -- see docs/Rational.md in this repo. +, -, *, / each accept a Rational, int, or
 * float operand on either side. When no float is involved, an int operand widens to Rational and
 * the result is an exact Rational, equivalent to calling the same-named method
 * (add()/sub()/mul()/div()) and throwing the same exceptions under the same conditions. When
 * either operand is a genuine float, both widen to float and the result is an ordinary float --
 * something none of the equivalent methods support. ** has its own rules: `Rational ** int` is
 * exact (equivalent to pow(int $exponent)); `Rational ** float` and `float ** Rational` both
 * widen to float; `Rational ** Rational` and `int ** Rational` are unsupported.
 */
#[CoversClass(Rational::class)]
class RationalBinaryOperatorsTest extends TestCase
{
    #region Operator + (add) tests.

    /**
     * Test Rational + Rational.
     */
    public function testAddRationalPlusRational(): void
    {
        $r1 = new Rational(1, 2);
        $r2 = new Rational(1, 3);
        $result = $r1 + $r2;

        $this->assertSame(5, $result->numerator);
        $this->assertSame(6, $result->denominator);
        $this->assertEquals($r1->add($r2), $result);
    }

    /**
     * Test Rational + int.
     */
    public function testAddRationalPlusInt(): void
    {
        $r = new Rational(1, 2);
        $result = $r + 1;

        $this->assertSame(3, $result->numerator);
        $this->assertSame(2, $result->denominator);
    }

    /**
     * Test Rational + float: both operands widen to float, unlike add(), which doesn't accept a
     * float operand at all.
     */
    public function testAddRationalPlusFloat(): void
    {
        $r = new Rational(1, 2);
        $result = $r + 0.25;

        $this->assertSame(0.75, $result);
    }

    /**
     * Test int|float + Rational gives the same result as Rational + int|float (commutative).
     */
    public function testAddScalarPlusRational(): void
    {
        $r = new Rational(1, 2);

        $this->assertEquals($r + 1, 1 + $r);
        $this->assertEquals($r + 0.25, 0.25 + $r);
    }

    /**
     * Test + does not modify either operand (immutability).
     */
    public function testAddDoesNotMutate(): void
    {
        $r1 = new Rational(1, 2);
        $r2 = new Rational(1, 3);

        $r3 = $r1 + $r2;

        $this->assertSame(1, $r1->numerator);
        $this->assertSame(2, $r1->denominator);
        $this->assertSame(1, $r2->numerator);
        $this->assertSame(3, $r2->denominator);
    }

    #endregion

    #region Operator - (subtract) tests.

    /**
     * Test Rational - Rational.
     */
    public function testSubRationalMinusRational(): void
    {
        $r1 = new Rational(3, 4);
        $r2 = new Rational(1, 4);
        $result = $r1 - $r2;

        $this->assertSame(1, $result->numerator);
        $this->assertSame(2, $result->denominator);
        $this->assertEquals($r1->sub($r2), $result);
    }

    /**
     * Test Rational - int: widens the int to Rational, result stays an exact Rational.
     */
    public function testSubRationalMinusInt(): void
    {
        $r = new Rational(3, 4);

        $result = $r - 1;
        $this->assertSame(-1, $result->numerator);
        $this->assertSame(4, $result->denominator);
    }

    /**
     * Test Rational - float: both operands widen to float, unlike sub(), which doesn't accept a
     * float operand at all.
     */
    public function testSubRationalMinusFloat(): void
    {
        $r = new Rational(3, 4);
        $result = $r - 0.25;

        $this->assertSame(0.5, $result);
    }

    /**
     * Test int - Rational: not commutative with Rational - int, the int is widened to a Rational
     * first, then subtracted normally.
     */
    public function testSubIntMinusRational(): void
    {
        $r = new Rational(3, 4);
        $result = 1 - $r;

        $this->assertSame(1, $result->numerator);
        $this->assertSame(4, $result->denominator);
    }

    /**
     * Test float - Rational: both operands widen to float.
     */
    public function testSubFloatMinusRational(): void
    {
        $r = new Rational(1, 4);
        $result = 1.0 - $r;

        $this->assertSame(0.75, $result);
    }

    /**
     * Test - does not modify either operand (immutability).
     */
    public function testSubDoesNotMutate(): void
    {
        $r1 = new Rational(3, 4);
        $r2 = new Rational(1, 4);

        $r3 = $r1 - $r2;

        $this->assertSame(3, $r1->numerator);
        $this->assertSame(4, $r1->denominator);
        $this->assertSame(1, $r2->numerator);
        $this->assertSame(4, $r2->denominator);
    }

    #endregion

    #region Operator * (multiply) tests.

    /**
     * Test Rational * Rational.
     */
    public function testMulRationalTimesRational(): void
    {
        $r1 = new Rational(2, 3);
        $r2 = new Rational(3, 4);
        $result = $r1 * $r2;

        $this->assertSame(1, $result->numerator);
        $this->assertSame(2, $result->denominator);
        $this->assertEquals($r1->mul($r2), $result);
    }

    /**
     * Test Rational * int: widens the int to Rational, result stays an exact Rational.
     */
    public function testMulRationalTimesInt(): void
    {
        $r = new Rational(2, 3);
        $result = $r * 3;

        $this->assertSame(2, $result->numerator);
        $this->assertSame(1, $result->denominator);
    }

    /**
     * Test Rational * float: both operands widen to float, unlike mul(), which doesn't accept a
     * float operand at all.
     */
    public function testMulRationalTimesFloat(): void
    {
        $r = new Rational(2, 3);
        $result = $r * 1.5;

        $this->assertSame(1.0, $result);
    }

    /**
     * Test int|float * Rational gives the same result as Rational * int|float (commutative).
     */
    public function testMulScalarTimesRational(): void
    {
        $r = new Rational(2, 3);

        $this->assertEquals($r * 3, 3 * $r);
        $this->assertEquals($r * 1.5, 1.5 * $r);
    }

    /**
     * Test * does not modify either operand (immutability).
     */
    public function testMulDoesNotMutate(): void
    {
        $r1 = new Rational(2, 3);
        $r2 = new Rational(3, 4);

        $r3 = $r1 * $r2;

        $this->assertSame(2, $r1->numerator);
        $this->assertSame(3, $r1->denominator);
        $this->assertSame(3, $r2->numerator);
        $this->assertSame(4, $r2->denominator);
    }

    #endregion

    #region Operator / (divide) tests.

    /**
     * Test Rational / Rational.
     */
    public function testDivRationalOverRational(): void
    {
        $r1 = new Rational(2, 3);
        $r2 = new Rational(3, 4);
        $result = $r1 / $r2;

        $this->assertSame(8, $result->numerator);
        $this->assertSame(9, $result->denominator);
        $this->assertEquals($r1->div($r2), $result);
    }

    /**
     * Test Rational / int: widens the int to Rational, result stays an exact Rational.
     */
    public function testDivRationalOverInt(): void
    {
        $r = new Rational(2, 3);
        $result = $r / 2;

        $this->assertSame(1, $result->numerator);
        $this->assertSame(3, $result->denominator);
    }

    /**
     * Test Rational / float: both operands widen to float, unlike div(), which doesn't accept a
     * float operand at all.
     */
    public function testDivRationalOverFloat(): void
    {
        $r = new Rational(2, 3);
        $result = $r / 0.5;

        $this->assertEqualsWithDelta(4 / 3, $result, PHP_FLOAT_EPSILON);
    }

    /**
     * Test int / Rational: not commutative with Rational / int, the int is widened to a Rational
     * first, then divided normally -- this also gives a way to compute a multiplicative inverse
     * (see "Inverting" tests below).
     */
    public function testDivIntOverRational(): void
    {
        $r = new Rational(2, 3);
        $result = 1 / $r;

        $this->assertSame(3, $result->numerator);
        $this->assertSame(2, $result->denominator);
    }

    /**
     * Test float / Rational: both operands widen to float.
     */
    public function testDivFloatOverRational(): void
    {
        $r = new Rational(2, 3);
        $result = 1.0 / $r;

        $this->assertSame(1.5, $result);
    }

    /**
     * Test division by int zero throws ArithmeticException, matching div().
     */
    public function testDivByIntZeroThrows(): void
    {
        $r = new Rational(3, 4);

        $this->expectException(ArithmeticException::class);
        $r / 0; // @phpstan-ignore binaryOp.invalid
    }

    /**
     * Test division by float 0.0 also throws ArithmeticException.
     */
    public function testDivByFloatZeroThrows(): void
    {
        $r = new Rational(3, 4);

        $this->expectException(ArithmeticException::class);
        $result = $r / 0.0; // @phpstan-ignore binaryOp.invalid
    }

    /**
     * Test division by zero-numerator Rational throws ArithmeticException, matching div().
     */
    public function testDivByZeroRationalThrows(): void
    {
        $r = new Rational(3, 4);

        $this->expectException(ArithmeticException::class);
        $r / new Rational(0);
    }

    /**
     * Test / does not modify either operand (immutability).
     */
    public function testDivDoesNotMutate(): void
    {
        $r1 = new Rational(2, 3);
        $r2 = new Rational(3, 4);

        $r3 = $r1 / $r2;

        $this->assertSame(2, $r1->numerator);
        $this->assertSame(3, $r1->denominator);
        $this->assertSame(3, $r2->numerator);
        $this->assertSame(4, $r2->denominator);
    }

    #endregion

    #region Operator ** (power) tests.

    /**
     * Test Rational ** int.
     */
    public function testPowRationalToInt(): void
    {
        $r = new Rational(2, 3);
        $result = $r ** 2;

        $this->assertSame(4, $result->numerator);
        $this->assertSame(9, $result->denominator);
        $this->assertEquals($r->pow(2), $result);
    }

    /**
     * Test Rational ** negative int: raising to a negative power is equivalent to inverting first.
     */
    public function testPowRationalToNegativeInt(): void
    {
        $r = new Rational(2, 3);
        $result = $r ** -2;

        $this->assertSame(9, $result->numerator);
        $this->assertSame(4, $result->denominator);
    }

    /**
     * Test Rational ** float: both operands widen to float, unlike pow(), which only accepts an
     * int exponent.
     */
    public function testPowRationalToFloat(): void
    {
        $r = new Rational(4, 1);
        $result = $r ** 0.5;

        $this->assertSame(2.0, $result);
    }

    /**
     * Test float ** Rational: both operands widen to float.
     */
    public function testPowFloatToRational(): void
    {
        $r = new Rational(1, 2);
        $result = 4.0 ** $r;

        $this->assertSame(2.0, $result);
    }

    /**
     * Test int ** Rational is unsupported: int only ever widens to Rational, never straight to
     * float, so an int base has nowhere to go once the exponent is Rational (see
     * docs/Rational.md's "** (power)" section).
     */
    public function testPowIntToRationalThrows(): void
    {
        $r = new Rational(2, 3);

        $this->expectException(TypeError::class);
        $result = 2 ** $r; // @phpstan-ignore binaryOp.invalid
    }

    /**
     * Test Rational ** Rational is unsupported -- a Rational exponent has no exact Rational
     * result, and there's no float operand present to trigger the float-widening pathway either.
     */
    public function testPowRationalToRationalThrows(): void
    {
        $r1 = new Rational(2, 3);
        $r2 = new Rational(1, 2);

        $this->expectException(TypeError::class);
        $result = $r1 ** $r2;
    }

    /**
     * Test ** does not modify the operand (immutability).
     */
    public function testPowDoesNotMutate(): void
    {
        $r = new Rational(2, 3);

        $r2 = $r ** 2;

        $this->assertSame(2, $r->numerator);
        $this->assertSame(3, $r->denominator);
    }

    #endregion

    #region Inverting with 1 / r or r ** -1 tests.

    /**
     * Test that 1 / $r, $r ** -1, and $r->inv() all give the same multiplicative inverse.
     */
    public function testInvertingEquivalence(): void
    {
        $r = new Rational(3, 4);

        $inv = $r->inv();
        $viaDivision = 1 / $r;
        $viaPower = $r ** -1;

        $this->assertSame(4, $inv->numerator);
        $this->assertSame(3, $inv->denominator);
        $this->assertEquals($inv, $viaDivision);
        $this->assertEquals($inv, $viaPower);
    }

    /**
     * Test that inverting by division throws ArithmeticException for a zero-numerator operand, matching inv().
     */
    public function testInvertingZeroByDivThrows(): void
    {
        $zero = new Rational(0);

        $this->expectException(ArithmeticException::class);
        $r = 1 / $zero;
    }

    /**
     * Test that inverting via exponentiation throws ArithmeticException for a zero-numerator operand, matching inv().
     */
    public function testInvertingZeroByPowThrows(): void
    {
        $zero = new Rational(0);

        $this->expectException(ArithmeticException::class);
        $r = $zero ** -1;
    }

    #endregion
}
