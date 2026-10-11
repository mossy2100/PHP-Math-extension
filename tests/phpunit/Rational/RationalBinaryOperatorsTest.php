<?php

declare(strict_types=1);

namespace OceanMoon\Math\Tests\Rational;

use DomainException;
use OceanMoon\Core\Exceptions\ArithmeticException;
use OceanMoon\Math\Rational;
use OverflowException;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\TestCase;

/**
 * Tests for Rational's binary arithmetic operator overloads (+, -, *, /, **), only available in
 * the extension - see docs/Rational.md in this repo. +, -, *, / each accept a Rational, int, or
 * float operand on either side. When no float is involved, an int operand widens to Rational and
 * the result is an exact Rational, equivalent to calling the same-named method
 * (add()/sub()/mul()/div()) and throwing the same exceptions under the same conditions. When
 * either operand is a genuine float, both widen to float and the result is an ordinary float -
 * something none of the equivalent methods support. ** follows the same rule: `Rational ** int`,
 * `Rational ** Rational` and `int ** Rational` are exact, and throw if the result is irrational;
 * `Rational ** float` and `float ** Rational` are floats. `Rational ** int` and
 * `Rational ** Rational` are equivalent to pow(). Unlike PHP's own float arithmetic, every float form throws
 * DomainException for a non-finite operand and OverflowException for an overflowing result, and never gives NAN.
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

    /**
     * Test a non-finite float operand throws, rather than following float arithmetic.
     */
    public function testAddNonFiniteOperandThrows(): void
    {
        $this->expectException(DomainException::class);
        $this->expectExceptionMessage('Invalid operand: NAN. Must be finite.');
        $result = new Rational(1, 2) + NAN;
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

    /**
     * Test a non-finite float operand throws, rather than following float arithmetic.
     */
    public function testSubNonFiniteOperandThrows(): void
    {
        $this->expectException(DomainException::class);
        $this->expectExceptionMessage('Invalid operand: NAN. Must be finite.');
        $result = NAN - new Rational(1, 2);
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

    /**
     * Test a non-finite float operand throws, rather than following float arithmetic.
     */
    public function testMulNonFiniteOperandThrows(): void
    {
        $this->expectException(DomainException::class);
        $this->expectExceptionMessage('Invalid operand: NAN. Must be finite.');
        $result = new Rational(1, 2) * NAN;
    }

    /**
     * Test a float result that overflows throws.
     */
    public function testMulFloatOverflowThrows(): void
    {
        $this->expectException(OverflowException::class);
        $this->expectExceptionMessage('Cannot multiply Rational and float. The result would overflow.');
        $result = new Rational(PHP_INT_MAX) * 1e300;
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
     * first, then divided normally - this also gives a way to compute a multiplicative inverse
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

    /**
     * Test a non-finite float operand throws, rather than following float arithmetic.
     */
    public function testDivNonFiniteOperandThrows(): void
    {
        $this->expectException(DomainException::class);
        $this->expectExceptionMessage('Invalid operand: NAN. Must be finite.');
        $result = NAN / new Rational(1, 2);
    }

    /**
     * Test a float result that overflows throws.
     */
    public function testDivFloatOverflowThrows(): void
    {
        $this->expectException(OverflowException::class);
        $this->expectExceptionMessage('Cannot divide Rational and float. The result would overflow.');
        $result = new Rational(PHP_INT_MAX) / 1e-300;
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
     * Test Rational ** float gives a float.
     */
    public function testPowRationalToFloat(): void
    {
        $this->assertSame(2.0, new Rational(4, 1) ** 0.5);
    }

    /**
     * Test Rational ** float throws for a zero base with a negative exponent, the same as pow().
     */
    public function testPowZeroRationalToNegativeFloatThrows(): void
    {
        $this->expectException(ArithmeticException::class);
        $result = new Rational(0) ** -0.5;
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
     * Test Rational ** float throws where PHP's own ** would give NAN from finite operands.
     */
    public function testPowRationalToFloatNotRealThrows(): void
    {
        $this->expectException(DomainException::class);
        $this->expectExceptionMessage('Cannot raise -1/4 to power 0.5. The result is not real.');
        $result = new Rational(-1, 4) ** 0.5;
    }

    /**
     * Test Rational ** float throws when the result overflows.
     */
    public function testPowRationalToFloatOverflowThrows(): void
    {
        $this->expectException(OverflowException::class);
        $this->expectExceptionMessage('Cannot raise Rational to a float power. The result would overflow.');
        $result = new Rational(PHP_INT_MAX) ** 100.0;
    }

    /**
     * Data for float ** Rational.
     *
     * @return array<string, array{float, int, int, float}> Base, exponent numerator, exponent
     * denominator, expected result.
     */
    public static function floatToRationalProvider(): array
    {
        return [
            'square root'          => [2.0, 1, 2, M_SQRT2],
            'root then power'      => [27.0, 2, 3, 9.0],
            'zero exponent'        => [2.5, 0, 1, 1.0],
            'zero base'            => [0.0, 1, 2, 0.0],
            'zero to zero'         => [0.0, 0, 1, 1.0],
            'odd root of negative' => [-8.0, 1, 3, -2.0],
            'odd root, even power' => [-8.0, 2, 3, 4.0],
            'negative exponent'    => [4.0, -1, 2, 0.5],
        ];
    }

    /**
     * Test float ** Rational gives a float. Unlike PHP's own float ** float, an odd root of a
     * negative base is real.
     */
    #[DataProvider('floatToRationalProvider')]
    public function testPowFloatToRationalValues(float $base, int $p, int $q, float $expected): void
    {
        $this->assertEqualsWithDelta($expected, $base ** new Rational($p, $q), EPSILON);
    }

    /**
     * Test float ** Rational throws for an even root of a negative base, rather than giving NAN.
     */
    public function testPowNegativeFloatToEvenRootThrows(): void
    {
        $this->expectException(DomainException::class);
        $this->expectExceptionMessage('Cannot raise -4.0 to power 1/2. The result is not real.');
        $result = (-4.0) ** new Rational(1, 2);
    }

    /**
     * Test float ** Rational throws for a zero base with a negative exponent, rather than giving
     * INF.
     */
    public function testPowZeroFloatToNegativeRationalThrows(): void
    {
        $this->expectException(ArithmeticException::class);
        $result = 0.0 ** new Rational(-1, 2);
    }

    /**
     * Data for Rational ** float with a non-finite exponent.
     *
     * @return array<string, array{Rational, float, string}> Base, exponent, expected message.
     */
    public static function powRationalToNonFiniteProvider(): array
    {
        return [
            'NAN'  => [
                new Rational(1, 2),
                NAN,
                'Invalid operand: NAN. Must be finite.',
            ],
            'INF'  => [
                new Rational(2),
                INF,
                'Invalid operand: INF. Must be finite.',
            ],
            '-INF' => [
                new Rational(0),
                -INF,
                'Invalid operand: -INF. Must be finite.',
            ],
        ];
    }

    /**
     * Test Rational ** float throws for a non-finite exponent, rather than following float arithmetic.
     */
    #[DataProvider('powRationalToNonFiniteProvider')]
    public function testPowRationalToNonFiniteThrows(Rational $base, float $exp, string $message): void
    {
        $this->expectException(DomainException::class);
        $this->expectExceptionMessage($message);
        $result = $base ** $exp;
    }

    /**
     * Data for float ** Rational with a non-finite base.
     *
     * @return array<string, array{float, Rational, string}> Base, exponent, expected message.
     */
    public static function powNonFiniteToRationalProvider(): array
    {
        return [
            'NAN'  => [
                NAN,
                new Rational(0),
                'Invalid operand: NAN. Must be finite.',
            ],
            'INF'  => [
                INF,
                new Rational(1, 2),
                'Invalid operand: INF. Must be finite.',
            ],
            '-INF' => [
                -INF,
                new Rational(1, 2),
                'Invalid operand: -INF. Must be finite.',
            ],
        ];
    }

    /**
     * Test float ** Rational throws for a non-finite base, rather than following float arithmetic.
     */
    #[DataProvider('powNonFiniteToRationalProvider')]
    public function testPowNonFiniteToRationalThrows(float $base, Rational $exp, string $message): void
    {
        $this->expectException(DomainException::class);
        $this->expectExceptionMessage($message);
        $result = $base ** $exp;
    }

    /**
     * Test float ** Rational throws when the result overflows.
     */
    public function testPowFloatToRationalOverflowThrows(): void
    {
        $this->expectException(OverflowException::class);
        $this->expectExceptionMessage('Cannot raise float to a Rational power. The result would overflow.');
        $result = (-1e300) ** new Rational(3);
    }

    /**
     * Test Rational ** Rational gives the same result as pow(Rational).
     */
    public function testPowRationalToRational(): void
    {
        $base = new Rational(8, 27);
        $exp = new Rational(-2, 3);
        $result = $base ** $exp;

        $this->assertSame(9, $result->numerator);
        $this->assertSame(4, $result->denominator);
        $this->assertEquals($base->pow($exp), $result);
    }

    /**
     * Test Rational ** Rational throws when the result is irrational, the same as pow(Rational).
     */
    public function testPowRationalToRationalIrrationalThrows(): void
    {
        $this->expectException(DomainException::class);
        $this->expectExceptionMessage('Cannot raise 2/9 to power 1/2. The result is irrational.');
        $result = new Rational(2, 9) ** new Rational(1, 2);
    }

    /**
     * Test Rational ** Rational throws for an even root of a negative base, the same as
     * pow(Rational).
     */
    public function testPowNegativeRationalToEvenRootThrows(): void
    {
        $this->expectException(DomainException::class);
        $this->expectExceptionMessage('Cannot raise -4/9 to power 1/2. The result is not real.');
        $result = new Rational(-4, 9) ** new Rational(1, 2);
    }

    /**
     * Test Rational ** Rational throws for a zero base with a negative exponent, the same as
     * pow(Rational).
     */
    public function testPowZeroRationalToNegativeRationalThrows(): void
    {
        $this->expectException(ArithmeticException::class);
        $result = new Rational(0) ** new Rational(-1, 2);
    }

    /**
     * Data for int ** Rational with an exact result.
     *
     * @return array<string, array{int, int, int, int, int}> Base, exponent numerator, exponent
     * denominator, expected numerator, expected denominator.
     */
    public static function intToRationalProvider(): array
    {
        return [
            'square root'                 => [4, 1, 2, 2, 1],
            'cube root'                   => [27, 1, 3, 3, 1],
            'root then power'             => [8, 2, 3, 4, 1],
            'zero exponent'               => [5, 0, 1, 1, 1],
            'zero base'                   => [0, 1, 2, 0, 1],
            'base one'                    => [1, 7, 3, 1, 1],
            'odd root of negative'        => [-8, 1, 3, -2, 1],
            'odd root, even power'        => [-8, 2, 3, 4, 1],
            'integer exponent'            => [3, 4, 1, 81, 1],
            'negative integer exponent'   => [2, -1, 1, 1, 2],
            'negative square root'        => [4, -1, 2, 1, 2],
            'negative root then power'    => [8, -2, 3, 1, 4],
            'negative odd root, negative' => [-27, -1, 3, -1, 3],
            'largest square root'         => [
                3037000499 * 3037000499,
                1,
                2,
                3037000499,
                1,
            ],
            'PHP_INT_MIN cube root'       => [PHP_INT_MIN, 1, 3, -2097152, 1],
            'PHP_INT_MIN 63rd root'       => [PHP_INT_MIN, 1, 63, -2, 1],
        ];
    }

    /**
     * Test int ** Rational with an exact result gives a Rational, including a whole-number
     * result. The int base doesn't widen to a Rational first, so PHP_INT_MIN works too.
     */
    #[DataProvider('intToRationalProvider')]
    public function testPowIntToRational(int $base, int $p, int $q, int $num, int $den): void
    {
        $result = $base ** new Rational($p, $q);

        $this->assertSame($num, $result->numerator);
        $this->assertSame($den, $result->denominator);
    }

    /**
     * Data for int ** Rational with an irrational result.
     *
     * @return array<string, array{int, int, int}> Base, exponent numerator, exponent denominator.
     */
    public static function intToRationalIrrationalProvider(): array
    {
        return [
            'square root of 2'        => [2, 1, 2],
            'cube root of -2'         => [-2, 1, 3],
            'root degree 63 or more'  => [2, 1, 64],
            'PHP_INT_MAX square root' => [PHP_INT_MAX, 1, 2],
            'PHP_INT_MIN fifth root'  => [PHP_INT_MIN, 1, 5],
        ];
    }

    /**
     * Test int ** Rational throws when the result is irrational, rather than giving a float.
     */
    #[DataProvider('intToRationalIrrationalProvider')]
    public function testPowIntToRationalIrrationalThrows(int $base, int $p, int $q): void
    {
        $this->expectException(DomainException::class);
        $this->expectExceptionMessage('irrational');
        $result = $base ** new Rational($p, $q);
    }

    /**
     * Test int ** Rational throws for an even root of a negative base, whether or not the
     * magnitude has an exact root.
     */
    public function testPowNegativeIntToEvenRootThrows(): void
    {
        foreach ([-4, -2] as $base) {
            try {
                $result = $base ** new Rational(1, 2);
                $this->fail("Expected DomainException was not thrown for $base.");
            } catch (DomainException $e) {
                $this->assertSame("Cannot raise $base to power 1/2. The result is not real.", $e->getMessage());
            }
        }
    }

    /**
     * Test int ** Rational throws for a zero base with a negative exponent.
     */
    public function testPowZeroIntToNegativeRationalThrows(): void
    {
        $this->expectException(ArithmeticException::class);
        $result = 0 ** new Rational(-1, 2);
    }

    /**
     * Test int ** Rational throws when the exact result overflows an int.
     */
    public function testPowIntToRationalOverflowThrows(): void
    {
        // 4^(127/2) = 2^127, which doesn't fit in an int.
        $this->expectException(OverflowException::class);
        $result = 4 ** new Rational(127, 2);
    }

    /**
     * Test int ** Rational throws when the exact result can't be expressed as a Rational.
     */
    public function testPowIntToRationalUnrepresentableThrows(): void
    {
        // PHP_INT_MIN^1 is PHP_INT_MIN itself, which a Rational can't hold.
        $this->expectException(DomainException::class);
        $result = PHP_INT_MIN ** new Rational(1);
    }

    /**
     * Test int ** Rational throws when the result's denominator doesn't fit in an int, the same
     * as Rational ** int.
     */
    public function testPowIntToRationalDenominatorOverflowThrows(): void
    {
        // (-2)^-63 = 1/-2^63, and 2^63 doesn't fit in an int.
        $this->expectException(OverflowException::class);
        $result = (-2) ** new Rational(-63);
    }

    /**
     * Test ** does not modify the operand (immutability).
     */
    public function testPowDoesNotMutate(): void
    {
        $r1 = new Rational(2, 3);

        $r2 = $r1 ** 2;

        $this->assertSame(2, $r1->numerator);
        $this->assertSame(3, $r1->denominator);
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
