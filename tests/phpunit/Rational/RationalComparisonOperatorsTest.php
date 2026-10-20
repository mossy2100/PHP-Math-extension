<?php

declare(strict_types=1);

namespace OceanMoon\Math\Tests\Rational;

use OceanMoon\Math\Complex;
use OceanMoon\Math\Rational;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\TestCase;
use stdClass;

/**
 * Tests for Rational's comparison operator overloads (<, <=, >, >=, <=>, ==, !=), only available
 * in the extension - see docs/Rational.md in this repo. `Rational` is the only one of the four
 * classes to get comparison operators, since it has a genuine natural ordering (unlike Complex,
 * Vector, and Matrix). All six operators are backed by a single native `compare` object handler,
 * equivalent to compare() - PHP has no way to wire up a subset of these independently, and
 * ==/!= come along as an unavoidable side effect of that (this project's own conventions still
 * prefer equal()/approxCompare() over ==/!= in new code, per docs/Rational.md).
 */
#[CoversClass(Rational::class)]
class RationalComparisonOperatorsTest extends TestCase
{
    #region Operator <=> (spaceship) tests.

    /**
     * Test <=> with two Rationals.
     */
    public function testSpaceshipWithRational(): void
    {
        $r1 = new Rational(1, 2);
        $r2 = new Rational(1, 3);

        $this->assertSame(1, $r1 <=> $r2);
        $this->assertSame(-1, $r2 <=> $r1);
        $this->assertSame(0, $r1 <=> new Rational(1, 2));
    }

    /**
     * Test <=> with int|float on either side.
     */
    public function testSpaceshipWithScalar(): void
    {
        $r = new Rational(1, 2);

        $this->assertSame(0, $r <=> 0.5);
        $this->assertSame(0, 0.5 <=> $r);
        $this->assertSame(-1, $r <=> 1);
        $this->assertSame(1, 1 <=> $r);
    }

    /**
     * Test <=> and == with NAN treat it as uncomparable, as PHP does for a float.
     */
    public function testSpaceshipWithNanIsUncomparable(): void
    {
        $r = new Rational(1, 2);

        $this->assertSame(1, $r <=> NAN);
        $this->assertFalse($r == NAN);
        $this->assertFalse(NAN == $r);
        $this->assertFalse($r < NAN);
    }

    /**
     * Test <=> and == with an unsupported operand treat it as uncomparable rather than throwing, in either order.
     */
    public function testSpaceshipUnsupportedOperandIsUncomparable(): void
    {
        $r = new Rational(1, 2);
        $operands = [
            'string',
            '0.5',
            [1],
            new stdClass(),
            new Complex(1, 2),
        ];

        foreach ($operands as $operand) {
            $this->assertSame(1, $r <=> $operand);
            $this->assertFalse($r == $operand);
            $this->assertFalse($operand == $r);
            $this->assertTrue($r != $operand);
        }
    }

    #endregion

    #region Operator < tests.

    /**
     * Test < with Rational and with a scalar on either side.
     */
    public function testLessThan(): void
    {
        $r1 = new Rational(1, 3);
        $r2 = new Rational(1, 2);

        $this->assertTrue($r1 < $r2);
        $this->assertFalse($r2 < $r1);
        $this->assertTrue($r1 < 0.5);
        $this->assertFalse(0.5 < $r1);
    }

    #endregion

    #region Operator <= tests.

    /**
     * Test <= with equal and unequal values.
     */
    public function testLessThanOrEqual(): void
    {
        $r1 = new Rational(1, 3);
        $r2 = new Rational(1, 2);

        $this->assertTrue($r1 <= $r2);
        $this->assertTrue($r1 <= new Rational(1, 3));
        $this->assertFalse($r2 <= $r1);
    }

    #endregion

    #region Operator > tests.

    /**
     * Test > with Rational and with a scalar on either side.
     */
    public function testGreaterThan(): void
    {
        $r1 = new Rational(3, 4);
        $r2 = new Rational(1, 2);

        $this->assertTrue($r1 > $r2);
        $this->assertFalse($r2 > $r1);
        $this->assertTrue(0.6 > $r2);
    }

    #endregion

    #region Operator >= tests.

    /**
     * Test >= with equal and unequal values.
     */
    public function testGreaterThanOrEqual(): void
    {
        $r1 = new Rational(3, 4);
        $r2 = new Rational(1, 2);

        $this->assertTrue($r1 >= $r2);
        $this->assertTrue($r1 >= new Rational(3, 4));
        $this->assertFalse($r2 >= $r1);
    }

    #endregion

    #region Operator == tests.

    /**
     * Test == uses exact value comparison, not identity - two distinct instances representing
     * the same value are ==, matching equal().
     */
    public function testEqual(): void
    {
        $r1 = new Rational(1, 2);
        $r2 = new Rational(2, 4);
        $r3 = new Rational(1, 3);

        $this->assertTrue($r1 == $r2);
        $this->assertNotSame($r1, $r2);
        $this->assertFalse($r1 == $r3);
    }

    /**
     * Test == with a scalar operand.
     */
    public function testEqualWithScalar(): void
    {
        $r = new Rational(1, 2);

        $this->assertTrue($r == 0.5);
        $this->assertFalse($r == 0.6);
    }

    /**
     * Test in_array() on a mixed array, which compares with == and so used to throw for a string element.
     */
    public function testEqualInMixedArray(): void
    {
        $r = new Rational(1, 2);

        $this->assertTrue(in_array($r, [
            null,
            'a',
            [1],
            new Rational(1, 2),
        ], false));
        $this->assertFalse(in_array($r, [
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
        $r1 = new Rational(1, 2);
        $r2 = new Rational(2, 4);
        $r3 = new Rational(1, 3);

        $this->assertFalse($r1 != $r2);
        $this->assertTrue($r1 != $r3);
    }

    #endregion
}
