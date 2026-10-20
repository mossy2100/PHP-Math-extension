<?php

declare(strict_types=1);

namespace OceanMoon\Math\Tests\Complex;

use OceanMoon\Math\Complex;
use OceanMoon\Math\Rational;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\TestCase;
use stdClass;

/**
 * Tests for Complex's comparison operator overloads (<, <=, >, >=, <=>, ==, !=), only available in
 * the extension - see docs/Complex.md in this repo. Comparing complex numbers isn't mathematically
 * meaningful (there's no total order compatible with the field operations), so these use plain
 * lexicographic (real, then imaginary) ordering - exactly what PHP's own default object comparison
 * already gives two Complex instances for free, since real is declared before imaginary. The only
 * genuinely new behavior is accepting an int|float operand on either side, promoted the same way
 * equal() promotes one. All six operators are backed by a single native `compare` object handler -
 * PHP has no way to wire up a subset of these independently, and ==/!= come along as an unavoidable
 * side effect of that.
 */
#[CoversClass(Complex::class)]
class ComplexComparisonOperatorsTest extends TestCase
{
    #region Operator <=> (spaceship) tests.

    /**
     * Test <=> compares real parts first.
     */
    public function testSpaceshipComparesRealFirst(): void
    {
        $z1 = new Complex(1, 5);
        $z2 = new Complex(2, 1);

        $this->assertSame(-1, $z1 <=> $z2);
        $this->assertSame(1, $z2 <=> $z1);
    }

    /**
     * Test <=> falls back to comparing imaginary parts when real parts are equal.
     */
    public function testSpaceshipFallsBackToImaginary(): void
    {
        $z1 = new Complex(3, 1);
        $z2 = new Complex(3, 5);

        $this->assertSame(-1, $z1 <=> $z2);
        $this->assertSame(1, $z2 <=> $z1);
        $this->assertSame(0, $z1 <=> new Complex(3, 1));
    }

    /**
     * Test <=> with int|float on either side.
     */
    public function testSpaceshipWithScalar(): void
    {
        $z = new Complex(5, 0);

        $this->assertSame(0, $z <=> 5);
        $this->assertSame(0, 5 <=> $z);
        $this->assertSame(-1, $z <=> 6);
        $this->assertSame(1, 6 <=> $z);
    }

    /**
     * Test <=> with a non-real Complex against a scalar compares as if the scalar had a zero
     * imaginary part.
     */
    public function testSpaceshipWithScalarAndNonZeroImaginary(): void
    {
        $z = new Complex(5, 1);

        // Real parts equal (5 == 5); imaginary 1 > 0.
        $this->assertSame(1, $z <=> 5);
        $this->assertSame(-1, 5 <=> $z);
    }

    /**
     * Test <=> with +-INF: a Complex is always finite, so it's always less than +INF and always
     * greater than -INF.
     */
    public function testSpaceshipWithInfinity(): void
    {
        $z = new Complex(5, 0);

        $this->assertSame(-1, $z <=> INF);
        $this->assertSame(1, INF <=> $z);
        $this->assertSame(1, $z <=> -INF);
        $this->assertSame(-1, -INF <=> $z);
    }

    /**
     * Test <=> and == with NAN treat it as uncomparable, as PHP does for a float.
     */
    public function testSpaceshipWithNanIsUncomparable(): void
    {
        $z = new Complex(1, 2);

        $this->assertSame(1, $z <=> NAN);
        $this->assertFalse($z == NAN);
        $this->assertFalse(NAN == $z);
        $this->assertFalse($z < NAN);
    }

    /**
     * Test <=> and == with an unsupported operand treat it as uncomparable rather than throwing, in either order.
     */
    public function testSpaceshipUnsupportedOperandIsUncomparable(): void
    {
        $z = new Complex(1, 2);
        $operands = [
            'string',
            '0.5',
            [1],
            new stdClass(),
            new Rational(1, 2),
        ];

        foreach ($operands as $operand) {
            $this->assertSame(1, $z <=> $operand);
            $this->assertFalse($z == $operand);
            $this->assertFalse($operand == $z);
            $this->assertTrue($z != $operand);
        }
    }

    #endregion

    #region Operator < tests.

    /**
     * Test < with Complex and with a scalar on either side.
     */
    public function testLessThan(): void
    {
        $z1 = new Complex(1, 0);
        $z2 = new Complex(2, 0);

        $this->assertTrue($z1 < $z2);
        $this->assertFalse($z2 < $z1);
        $this->assertTrue($z1 < 1.5);
        $this->assertFalse(1.5 < $z1);
    }

    #endregion

    #region Operator <= tests.

    /**
     * Test <= with equal and unequal values.
     */
    public function testLessThanOrEqual(): void
    {
        $z1 = new Complex(1, 0);
        $z2 = new Complex(2, 0);

        $this->assertTrue($z1 <= $z2);
        $this->assertTrue($z1 <= new Complex(1, 0));
        $this->assertFalse($z2 <= $z1);
    }

    #endregion

    #region Operator > tests.

    /**
     * Test > with Complex and with a scalar on either side.
     */
    public function testGreaterThan(): void
    {
        $z1 = new Complex(3, 0);
        $z2 = new Complex(2, 0);

        $this->assertTrue($z1 > $z2);
        $this->assertFalse($z2 > $z1);
        $this->assertTrue(2.5 > $z2);
    }

    #endregion

    #region Operator >= tests.

    /**
     * Test >= with equal and unequal values.
     */
    public function testGreaterThanOrEqual(): void
    {
        $z1 = new Complex(3, 0);
        $z2 = new Complex(2, 0);

        $this->assertTrue($z1 >= $z2);
        $this->assertTrue($z1 >= new Complex(3, 0));
        $this->assertFalse($z2 >= $z1);
    }

    #endregion

    #region Operator == tests.

    /**
     * Test == uses exact value comparison, not identity - two distinct instances representing
     * the same value are ==, matching equal().
     */
    public function testEqual(): void
    {
        $z1 = new Complex(3, 4);
        $z2 = new Complex(3, 4);
        $other = new Complex(3, 5);

        $this->assertTrue($z1 == $z2);
        $this->assertNotSame($z1, $z2);
        $this->assertFalse($z1 == $other);
    }

    /**
     * Test == with a scalar operand.
     */
    public function testEqualWithScalar(): void
    {
        $z = new Complex(5, 0);

        $this->assertTrue($z == 5);
        $this->assertFalse($z == 6);
    }

    /**
     * Test in_array() on a mixed array, which compares with == and so used to throw for a string element.
     */
    public function testEqualInMixedArray(): void
    {
        $z = new Complex(1, 2);

        $this->assertTrue(in_array($z, [
            null,
            'a',
            [1],
            new Complex(1, 2),
        ], false));
        $this->assertFalse(in_array($z, [
            null,
            'a',
            [1],
        ], false));
    }

    #endregion

    #region Operator != tests.

    /**
     * Test != is the negation of ==.
     */
    public function testNotEqual(): void
    {
        $z1 = new Complex(3, 4);
        $z2 = new Complex(3, 4);
        $other = new Complex(3, 5);

        $this->assertFalse($z1 != $z2);
        $this->assertTrue($z1 != $other);
    }

    #endregion
}
