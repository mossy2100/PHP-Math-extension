<?php

declare(strict_types=1);

namespace OceanMoon\Math\Tests\Rational;

use DomainException;
use InvalidArgumentException;
use OceanMoon\Math\Rational;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\TestCase;

/**
 * Tests for Rational's comparison operator overloads (<, <=, >, >=, <=>, ==, !=), only available
 * in the extension -- see docs/Rational.md in this repo. `Rational` is the only one of the four
 * classes to get comparison operators, since it has a genuine natural ordering (unlike Complex,
 * Vector, and Matrix). All six operators are backed by a single native `compare` object handler,
 * equivalent to compare() -- PHP has no way to wire up a subset of these independently, and
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
     * Test <=> with an invalid type throws InvalidArgumentException, matching compare().
     */
    public function testSpaceshipInvalidTypeThrows(): void
    {
        $r = new Rational(1, 2);

        $this->expectException(InvalidArgumentException::class);
        $cmp = $r <=> 'string';
    }

    /**
     * Test <=> with NAN throws DomainException, matching compare().
     */
    public function testSpaceshipWithNanThrows(): void
    {
        $r = new Rational(1, 2);

        $this->expectException(DomainException::class);
        $cmp = $r <=> NAN;
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
     * Test == uses exact value comparison, not identity -- two distinct instances representing
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
