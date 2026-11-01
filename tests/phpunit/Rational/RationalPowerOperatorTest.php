<?php

declare(strict_types=1);

namespace Mossy2100\Math\Tests\Rational;

use DomainException;
use Mossy2100\Core\Exceptions\ArithmeticException;
use Mossy2100\Math\Rational;
use OverflowException;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\TestCase;

/**
 * Tests for Rational's ** operator overload, only available in the extension - see docs/Rational.md in this repo.
 * `Rational ** int`, `Rational ** Rational` and `int ** Rational` are exact, equivalent to pow() where it applies, and
 * throw if the result is irrational. `Rational ** float` and `float ** Rational` are floats. Unlike PHP's own float
 * arithmetic, every float form throws DomainException for a non-finite operand and OverflowException for an
 * overflowing result, and never gives NAN.
 */
#[CoversClass(Rational::class)]
class RationalPowerOperatorTest extends TestCase
{
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
        $this->expectExceptionMessage(
            'Cannot raise the given Rational to the given float power. The result would overflow.'
        );
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
            'square root' => [2.0, 1, 2, M_SQRT2],
            'root then power' => [27.0, 2, 3, 9.0],
            'zero exponent' => [2.5, 0, 1, 1.0],
            'zero base' => [0.0, 1, 2, 0.0],
            'zero to zero' => [0.0, 0, 1, 1.0],
            'odd root of negative' => [-8.0, 1, 3, -2.0],
            'odd root, even power' => [-8.0, 2, 3, 4.0],
            'negative exponent' => [4.0, -1, 2, 0.5],
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
            'NAN' => [new Rational(1, 2), NAN, 'Invalid operand: NAN. Must be finite.'],
            'INF' => [new Rational(2), INF, 'Invalid operand: INF. Must be finite.'],
            '-INF' => [new Rational(0), -INF, 'Invalid operand: -INF. Must be finite.'],
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
            'NAN' => [NAN, new Rational(0), 'Invalid operand: NAN. Must be finite.'],
            'INF' => [INF, new Rational(1, 2), 'Invalid operand: INF. Must be finite.'],
            '-INF' => [-INF, new Rational(1, 2), 'Invalid operand: -INF. Must be finite.'],
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
        $this->expectExceptionMessage(
            'Cannot raise the given float to the given Rational power. The result would overflow.'
        );
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
            'square root' => [4, 1, 2, 2, 1],
            'cube root' => [27, 1, 3, 3, 1],
            'root then power' => [8, 2, 3, 4, 1],
            'zero exponent' => [5, 0, 1, 1, 1],
            'zero base' => [0, 1, 2, 0, 1],
            'base one' => [1, 7, 3, 1, 1],
            'odd root of negative' => [-8, 1, 3, -2, 1],
            'odd root, even power' => [-8, 2, 3, 4, 1],
            'integer exponent' => [3, 4, 1, 81, 1],
            'negative integer exponent' => [2, -1, 1, 1, 2],
            'negative square root' => [4, -1, 2, 1, 2],
            'negative root then power' => [8, -2, 3, 1, 4],
            'negative odd root, negative' => [-27, -1, 3, -1, 3],
            'largest square root' => [3037000499 * 3037000499, 1, 2, 3037000499, 1],
            'PHP_INT_MIN cube root' => [PHP_INT_MIN, 1, 3, -2097152, 1],
            'PHP_INT_MIN 63rd root' => [PHP_INT_MIN, 1, 63, -2, 1],
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
            'square root of 2' => [2, 1, 2],
            'cube root of -2' => [-2, 1, 3],
            'root degree 63 or more' => [2, 1, 64],
            'PHP_INT_MAX square root' => [PHP_INT_MAX, 1, 2],
            'PHP_INT_MIN fifth root' => [PHP_INT_MIN, 1, 5],
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
        $this->expectExceptionMessage(
            'Cannot raise the given int to the given Rational power. The result would overflow.'
        );
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
        $this->expectExceptionMessage(
            'Cannot raise the given int to the given Rational power. The result would overflow.'
        );
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

    /**
     * Test an exact result that overflows throws, with the same message as pow().
     */
    public function testPowOverflowThrows(): void
    {
        $this->expectException(OverflowException::class);
        $this->expectExceptionMessage('Cannot raise the given Rational to the given power. The result would overflow.');
        $result = new Rational(3) ** 50;
    }

    #endregion

    #region Inverting with 1 / r or r ** -1 tests.

    /**
     * Test that 1 / $r, $r ** -1, and $r->recip() all give the same multiplicative inverse.
     */
    public function testInvertingEquivalence(): void
    {
        $r = new Rational(3, 4);

        $recip = $r->recip();
        $viaDivision = 1 / $r;
        $viaPower = $r ** -1;

        $this->assertSame(4, $recip->numerator);
        $this->assertSame(3, $recip->denominator);
        $this->assertEquals($recip, $viaDivision);
        $this->assertEquals($recip, $viaPower);
    }

    /**
     * Test that inverting by division throws ArithmeticException for a zero-numerator operand, matching recip().
     */
    public function testInvertingZeroByDivThrows(): void
    {
        $zero = new Rational(0);

        $this->expectException(ArithmeticException::class);
        $r = 1 / $zero;
    }

    /**
     * Test that inverting via exponentiation throws ArithmeticException for a zero-numerator operand, matching recip().
     */
    public function testInvertingZeroByPowThrows(): void
    {
        $zero = new Rational(0);

        $this->expectException(ArithmeticException::class);
        $r = $zero ** -1;
    }

    #endregion
}
