<?php

declare(strict_types=1);

namespace OceanMoon\Math\Tests\Complex;

use OceanMoon\Math\Complex;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\TestCase;
use stdClass;

#[CoversClass(Complex::class)]
class ComplexComparisonOperatorsTest extends TestCase
{
    #region Exact equality tests

    /**
     * Test exact equality with identical complex numbers.
     */
    public function testEqualExact(): void
    {
        $z1 = new Complex(3, 4);
        $z2 = new Complex(3, 4);

        $this->assertTrue($z1 == $z2);
    }

    /**
     * Test inequality with different complex numbers.
     */
    public function testNotEqual(): void
    {
        $z1 = new Complex(3, 4);
        $z2 = new Complex(3, 5);

        $this->assertFalse($z1 == $z2);

        $z3 = new Complex(4, 4);
        $this->assertFalse($z1 == $z3);

        $z4 = new Complex(4, 5);
        $this->assertFalse($z1 == $z4);
    }

    /**
     * Test equality with real numbers (int and float).
     */
    public function testEqualWithRealNumber(): void
    {
        $z = new Complex(5, 0);

        // Should work with both int and float
        $this->assertTrue($z == 5);
        $this->assertTrue($z == 5.0);
        $this->assertFalse($z == 6);
        $this->assertFalse($z == 4.99999);
    }

    /**
     * Test equality with zero.
     */
    public function testEqualWithZero(): void
    {
        $z = new Complex(0, 0);

        $this->assertTrue($z == 0);
        $this->assertTrue($z == 0.0);
        $this->assertTrue($z == new Complex(0, 0));
        $this->assertFalse($z == new Complex(0, 1e-100));
    }

    /**
     * Test reflexivity: a value should equal itself.
     */
    public function testEqualReflexive(): void
    {
        $z1 = new Complex(3, 4);
        $this->assertTrue($z1 == $z1);

        $z2 = new Complex(-5.7, 2.3);
        $this->assertTrue($z2 == $z2);

        $z3 = new Complex(0, 0);
        $this->assertTrue($z3 == $z3);
    }

    /**
     * Test symmetry: if a equals b, then b equals a.
     */
    public function testEqualSymmetric(): void
    {
        $z1 = new Complex(3, 4);
        $z2 = new Complex(3, 4);

        $this->assertTrue($z1 == $z2);
        $this->assertTrue($z2 == $z1);

        $z3 = new Complex(5, 6);
        $z4 = new Complex(5, 7);

        $this->assertFalse($z3 == $z4);
        $this->assertFalse($z4 == $z3);
    }

    /**
     * Test transitivity: if a equals b and b equals c, then a equals c.
     */
    public function testEqualTransitive(): void
    {
        $z1 = new Complex(5, 6);
        $z2 = new Complex(5, 6);
        $z3 = new Complex(5, 6);

        $this->assertTrue($z1 == $z2);
        $this->assertTrue($z2 == $z3);
        $this->assertTrue($z1 == $z3);
    }

    /**
     * Test equality with negative zero.
     */
    public function testEqualNegativeZero(): void
    {
        // In PHP, -0.0 === 0.0 is true, so Complex should treat them as equal
        $z1 = new Complex(-0.0, 0);
        $z2 = new Complex(0.0, 0);

        $this->assertTrue($z1 == $z2);

        $z3 = new Complex(0, -0.0);
        $z4 = new Complex(0, 0.0);

        $this->assertTrue($z3 == $z4);

        $z5 = new Complex(-0.0, -0.0);
        $z6 = new Complex(0.0, 0.0);

        $this->assertTrue($z5 == $z6);
    }

    /**
     * Test that equal returns false for invalid types instead of throwing.
     */
    public function testEqualInvalidTypeReturnsFalse(): void
    {
        $z = new Complex(3, 4);

        $this->assertFalse($z == 'string');
        $this->assertFalse($z == null);
        $this->assertFalse($z == []);
        $this->assertFalse($z == new stdClass());
        $this->assertFalse($z == true);
    }

    /**
     * Test equality with pure imaginary numbers.
     */
    public function testEqualPureImaginary(): void
    {
        $z1 = new Complex(0, 5);
        $z2 = new Complex(0, 5);

        $this->assertTrue($z1 == $z2);

        $z3 = new Complex(0, -5);
        $this->assertFalse($z1 == $z3);
    }

    #endregion
}
