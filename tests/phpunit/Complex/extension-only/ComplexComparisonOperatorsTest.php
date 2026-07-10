<?php

declare(strict_types=1);

namespace OceanMoon\Math\Tests\Complex;

use OceanMoon\Math\Complex;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\TestCase;
use stdClass;

/**
 * Mirrors (in part) packages/Math's ComplexComparisonTest, but exercises the native `==`/`!=`
 * operators instead of calling equal() directly -- these can only be tested from the extension,
 * since userland PHP can't overload operators.
 *
 * identical() has no operator equivalent: === for objects is always identity (same instance),
 * hardcoded by the engine with no handler to override it, so it can't express identical()'s
 * value-based semantics for two distinct instances. approxEqual() likewise has no operator
 * equivalent. Neither is mirrored here.
 *
 * == has one gap of its own: comparing an object to bool (`$z == true`/`$z == false`) converts
 * both sides to bool first, without ever consulting the object's compare handler -- every object
 * is truthy, so this is always true for `true` and always false for `false`, for any PHP object.
 * See testEqualInvalidTypeReturnsFalse().
 */
#[CoversClass(Complex::class)]
class ComplexComparisonOperatorsTest extends TestCase
{
    #region Exact equality tests (==)

    /**
     * Test exact equality (==) with identical complex numbers.
     */
    public function testEqualExact(): void
    {
        $z1 = new Complex(3, 4);
        $z2 = new Complex(3, 4);

        $this->assertTrue($z1 == $z2);
    }

    /**
     * Test inequality (==) with different complex numbers.
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
     * Test equality (==) with real numbers (int and float).
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
     * Test equality (==) with zero.
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
     * Test reflexivity: a value should equal (==) itself.
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
     * Test symmetry: if a == b, then b == a.
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
     * Test transitivity: if a == b and b == c, then a == c.
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
     * Test equality (==) with negative zero.
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
     * Test that equality (==) returns false for invalid types instead of throwing.
     *
     * NB: doesn't test `$z == true` (unlike the equal() method's equivalent test) -- PHP compares
     * object == bool by converting *both sides* to bool first, never consulting the object's
     * compare handler. Since every object is truthy, `$z == true` is always true for any PHP
     * object; no C code can change that. `== null` is unaffected -- objects are never == null
     * regardless of any custom handler, so it still exercises the same "false for the wrong type"
     * behavior.
     */
    public function testEqualInvalidTypeReturnsFalse(): void
    {
        $z = new Complex(3, 4);

        $this->assertFalse($z == 'string');
        $this->assertFalse($z == null);
        $this->assertFalse($z == []);
        $this->assertFalse($z == new stdClass());
    }

    /**
     * Test equality (==) with a parseable string, converted via toComplex().
     */
    public function testEqualWithString(): void
    {
        $z = new Complex(3, 4);

        $this->assertTrue($z == '3+4i');
        $this->assertFalse($z == '3+5i');
        $this->assertFalse($z == 'not a number');
    }

    /**
     * Test equality (==) with a 2-element array (list or associative), converted via
     * toComplex().
     */
    public function testEqualWithArray(): void
    {
        $z = new Complex(3, 4);

        $this->assertTrue($z == [3, 4]);
        $this->assertTrue($z == ['real' => 3, 'imaginary' => 4]);
        $this->assertFalse($z == [3, 5]);
        $this->assertFalse($z == [1, 2, 3]);
    }

    /**
     * Test equality (==) with a plain object with numeric real/imaginary properties, converted
     * via toComplex().
     */
    public function testEqualWithObject(): void
    {
        $z = new Complex(3, 4);

        $this->assertTrue($z == (object) ['real' => 3, 'imaginary' => 4]);
        $this->assertFalse($z == (object) ['real' => 3, 'imaginary' => 5]);
    }

    /**
     * Test equality (==) with pure imaginary numbers.
     */
    public function testEqualPureImaginary(): void
    {
        $z1 = new Complex(0, 5);
        $z2 = new Complex(0, 5);

        $this->assertTrue($z1 == $z2);

        $z3 = new Complex(0, -5);
        $this->assertFalse($z1 == $z3);
    }

    /**
     * Test not equal (!=): the logical negation of == -- true for differing values or for
     * anything toComplex() can't convert, false when the values match (or are convertibly
     * equal).
     */
    public function testNotEqualOperator(): void
    {
        $z1 = new Complex(3, 4);

        $this->assertTrue($z1 != new Complex(3, 5));
        $this->assertTrue($z1 != 'not a number');
        $this->assertTrue($z1 != null);

        $z2 = new Complex(5, 0);
        $this->assertTrue($z2 != 6);
        $this->assertFalse($z2 != 5);
        $this->assertFalse($z2 != 5.0);

        $this->assertFalse($z1 != new Complex(3, 4));
        $this->assertFalse($z1 != '3+4i');
    }

    #endregion
}
