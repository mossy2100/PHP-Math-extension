<?php

declare(strict_types=1);

namespace Mossy2100\Math\Tests\Rational;

use DomainException;
use Mossy2100\Core\Exceptions\ArithmeticException;
use Mossy2100\Math\Rational;
use OverflowException;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\TestCase;
use TypeError;

/**
 * Tests for Rational's binary arithmetic operator overloads (+, -, *, /, %), only available in the extension - see
 * docs/Rational.md in this repo. Each accepts a Rational, int, or float operand on either side. When no float is
 * involved, an int operand widens to Rational and the result is an exact Rational, equivalent to calling the same-named
 * method (add()/sub()/mul()/div()/mod()) and throwing the same exceptions under the same conditions. When either
 * operand is a genuine float, both widen to float and the result is an ordinary float. Unlike PHP's own float
 * arithmetic, every float form throws DomainException for a non-finite operand and OverflowException for an
 * overflowing result, and never gives NAN. ** is tested in RationalPowerOperatorTest.
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

    /**
     * Test an exact result that overflows throws, with the same message as add().
     */
    public function testAddOverflowThrows(): void
    {
        $this->expectException(OverflowException::class);
        $this->expectExceptionMessage('Cannot add the given Rational numbers. The result would overflow.');
        $result = new Rational(PHP_INT_MAX) + 1;
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

    /**
     * Test an exact result that overflows throws, with the same message as sub().
     */
    public function testSubOverflowThrows(): void
    {
        $this->expectException(OverflowException::class);
        $this->expectExceptionMessage('Cannot subtract the given Rational numbers. The result would overflow.');
        $result = -2 - new Rational(PHP_INT_MAX);
    }

    /**
     * Test that a result of PHP_INT_MIN, which a Rational can't hold, throws DomainException.
     */
    public function testSubtractResultPhpIntMinThrows(): void
    {
        $this->expectException(DomainException::class);
        $this->expectExceptionMessage('Cannot express ratio -9223372036854775808/1 as Rational.');

        $result = new Rational(PHP_INT_MIN + 1) - 1;
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
        $this->expectExceptionMessage('Cannot multiply the given Rational and float. The result would overflow.');
        $result = new Rational(PHP_INT_MAX) * 1e300;
    }

    /**
     * Test an exact result that overflows throws, with the same message as mul().
     */
    public function testMulOverflowThrows(): void
    {
        $this->expectException(OverflowException::class);
        $this->expectExceptionMessage('Cannot multiply the given Rational numbers. The result would overflow.');
        $result = new Rational(PHP_INT_MAX) * 2;
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
        $this->expectExceptionMessage('Cannot divide the given Rational and float. The result would overflow.');
        $result = new Rational(PHP_INT_MAX) / 1e-300;
    }

    /**
     * Test an exact result that overflows throws, with the same message as div().
     */
    public function testDivOverflowThrows(): void
    {
        $this->expectException(OverflowException::class);
        $this->expectExceptionMessage('Cannot divide the given Rational numbers. The result would overflow.');
        $result = new Rational(PHP_INT_MAX) / new Rational(1, 2);
    }

    #endregion

    #region Operator % (modulo) tests.

    /**
     * Test Rational % Rational, equivalent to mod().
     */
    public function testModRationalModRational(): void
    {
        $r1 = new Rational(-7, 2);
        $r2 = new Rational(2, 3);
        $result = $r1 % $r2;

        $this->assertSame(-1, $result->numerator);
        $this->assertSame(6, $result->denominator);
        $this->assertEquals($r1->mod($r2), $result);
    }

    /**
     * Test Rational % int and int % Rational: the int widens to a Rational.
     */
    public function testModWithInt(): void
    {
        $result = new Rational(-7, 2) % 2;
        $this->assertSame(-3, $result->numerator);
        $this->assertSame(2, $result->denominator);

        $result = 7 % new Rational(2, 3);
        $this->assertSame(1, $result->numerator);
        $this->assertSame(3, $result->denominator);
    }

    /**
     * Test Rational % float and float % Rational: both widen to float, so the result is a float.
     */
    public function testModWithFloat(): void
    {
        $this->assertSame(-0.5, new Rational(-7, 2) % 1.5);
        $this->assertSame(1.0, 7.0 % new Rational(2));
    }

    /**
     * Test that the result has the sign of the dividend, like PHP's own %.
     */
    public function testModSignFollowsDividend(): void
    {
        $this->assertEquals(new Rational(3, 2), new Rational(7, 2) % -2);
        $this->assertEquals(new Rational(-3, 2), new Rational(-7, 2) % -2);
    }

    /**
     * Test % by zero throws ArithmeticException, for an int, a float and a Rational.
     */
    public function testModByZeroThrows(): void
    {
        $r = new Rational(3, 4);

        foreach ([0, 0.0, new Rational(0)] as $zero) {
            try {
                $result = $r % $zero;
                $this->fail('Expected ArithmeticException.');
            } catch (ArithmeticException $e) { // @phpstan-ignore catch.neverThrown
                $this->assertSame('Cannot divide by zero.', $e->getMessage());
            }
        }
    }

    /**
     * Test % with a non-finite float throws DomainException.
     */
    public function testModNonFiniteFloatThrows(): void
    {
        $this->expectException(DomainException::class);
        $this->expectExceptionMessage('Invalid operand: NAN. Must be finite.');

        $result = new Rational(3, 4) % NAN;
    }

    /**
     * Test % with large operands, where the remainder is representable, though the dividend over the common
     * denominator isn't. PHP_INT_MAX - 1 is divisible by 3, so 1/(PHP_INT_MAX - 1) divides PHP_INT_MAX/3 exactly.
     */
    public function testModLargeOperands(): void
    {
        $this->assertEquals(new Rational(0), new Rational(PHP_INT_MAX, 3) % new Rational(1, PHP_INT_MAX - 1));
    }

    /**
     * Test % throws OverflowException, with mod()'s message, when the remainder's denominator is too large.
     */
    public function testModOverflowThrows(): void
    {
        $this->expectException(OverflowException::class);
        $this->expectExceptionMessage(
            'Cannot find the remainder of the given Rational numbers. The result would overflow.'
        );

        // 4000000007 and 4000000009 are prime, so the remainder, 2/(4000000007 × 4000000009), is in lowest terms, and
        // its denominator is about 1.6e19.
        $result = new Rational(1, 4000000007) % new Rational(1, 4000000009);
    }

    /**
     * Test %= gives the same result as %.
     */
    public function testCompoundMod(): void
    {
        $r = new Rational(-7, 2);
        $original = $r;
        $r %= 2;

        $this->assertEquals(new Rational(-3, 2), $r);
        $this->assertEquals(new Rational(-7, 2), $original);
    }

    /**
     * Test % with an unsupported operand throws TypeError.
     */
    public function testModUnsupportedOperandThrows(): void
    {
        $this->expectException(TypeError::class);
        $this->expectExceptionMessage('Unsupported operand types: Mossy2100\Math\Rational % string');

        $result = new Rational(3, 4) % 'x'; // @phpstan-ignore binaryOp.invalid
    }

    /**
     * Test % throws TypeError for an operand PHP could convert to int, on either side, rather than truncating the
     * Rational to an int through its cast handler.
     */
    public function testModConvertibleUnsupportedOperandThrows(): void
    {
        $r = new Rational(7, 2);
        foreach (['2', true, false, null] as $operand) {
            $mods = [
                static fn () => $r % $operand, // @phpstan-ignore binaryOp.invalid
                static fn () => $operand % $r, // @phpstan-ignore binaryOp.invalid
            ];
            foreach ($mods as $mod) {
                try {
                    $mod();
                    $this->fail('Expected TypeError was not thrown for ' . var_export($operand, true) . '.');
                } catch (TypeError $e) {
                    $this->assertStringStartsWith('Unsupported operand types: ', $e->getMessage());
                }
            }
        }
    }

    #endregion

    #region Compound assignment tests.

    /**
     * Test that += gives the same result as +, leaving other references to the original unchanged (immutability).
     */
    public function testCompoundAdd(): void
    {
        $r = new Rational(1, 2);
        $original = $r;
        $r += new Rational(1, 3);

        $this->assertSame(5, $r->numerator);
        $this->assertSame(6, $r->denominator);
        $this->assertSame(1, $original->numerator);
        $this->assertSame(2, $original->denominator);
    }

    /**
     * Test that -= gives the same result as -.
     */
    public function testCompoundSubtract(): void
    {
        $r = new Rational(1, 2);
        $r -= new Rational(1, 3);

        $this->assertSame(1, $r->numerator);
        $this->assertSame(6, $r->denominator);
    }

    /**
     * Test that *= gives the same result as *.
     */
    public function testCompoundMultiply(): void
    {
        $r = new Rational(1, 2);
        $r *= new Rational(2, 3);

        $this->assertSame(1, $r->numerator);
        $this->assertSame(3, $r->denominator);
    }

    /**
     * Test that /= gives the same result as /.
     */
    public function testCompoundDivide(): void
    {
        $r = new Rational(1, 2);
        $r /= new Rational(1, 4);

        $this->assertSame(2, $r->numerator);
        $this->assertSame(1, $r->denominator);
    }

    /**
     * Test that **= gives the same result as **.
     */
    public function testCompoundPower(): void
    {
        $r = new Rational(1, 2);
        $r **= 2;

        $this->assertSame(1, $r->numerator);
        $this->assertSame(4, $r->denominator);
    }

    /**
     * Test that a float operand widens the result to float, as with +.
     */
    public function testCompoundAddFloat(): void
    {
        $r = new Rational(1, 2);
        $r += 0.25;

        $this->assertSame(0.75, $r);
    }

    /**
     * Test compound assignment with the Rational on the right, which turns an int variable into a Rational.
     */
    public function testCompoundIntTimesRational(): void
    {
        $x = 2;
        $x *= new Rational(1, 3);

        $this->assertInstanceOf(Rational::class, $x);
        $this->assertSame(2, $x->numerator);
        $this->assertSame(3, $x->denominator);
    }

    /**
     * Test compound assignment with the same variable on both sides.
     */
    public function testCompoundAddSelf(): void
    {
        $r = new Rational(1, 2);
        $r += $r;

        $this->assertSame(1, $r->numerator);
        $this->assertSame(1, $r->denominator);
    }

    /**
     * Test that a compound assignment that throws leaves the variable holding its original Rational.
     */
    public function testCompoundDivideByZeroLeavesVariableUnchanged(): void
    {
        $r = new Rational(1, 2);
        $original = $r;

        try {
            $r /= 0; // @phpstan-ignore assignOp.invalid
            $this->fail('Expected ArithmeticException.');
        } catch (ArithmeticException) { // @phpstan-ignore catch.neverThrown
            $this->assertSame($original, $r);
        }
    }

    #endregion

    #region Operators on clones tests.

    /**
     * Test that a cloned Rational keeps its operators.
     */
    public function testOperatorsOnClone(): void
    {
        $r = clone new Rational(1, 2);

        $product = $r * 3;
        $this->assertSame(3, $product->numerator);
        $this->assertSame(2, $product->denominator);

        $this->assertTrue($r < 1);
        $this->assertTrue($r == new Rational(1, 2));
    }

    #endregion
}
