<?php

declare(strict_types=1);

namespace OceanMoon\Math\Tests\Vector;

use OceanMoon\Math\Vector;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\TestCase;

/**
 * Tests for Vector's unary operator overloads (+, -), only available in the extension -- see
 * docs/Vector.md in this repo. PHP has no dedicated opcode for unary +/-; both lower to `$v * 1`/
 * `$v * -1`, so these exercise the ZEND_MUL case of the do_operation handler indirectly.
 */
#[CoversClass(Vector::class)]
class VectorUnaryOperatorsTest extends TestCase
{
    #region Operator + (identity) tests.

    /**
     * Test that unary + returns an equal Vector with the same elements.
     */
    public function testUnaryPlus(): void
    {
        $v1 = Vector::fromArray([1, 2, 3]);
        $v2 = +$v1;

        $this->assertSame([1.0, 2.0, 3.0], $v2->toArray());
    }

    /**
     * Test that unary + returns a new instance, not $v1 itself (Vector is mutable, so this matters).
     */
    public function testUnaryPlusReturnsNewInstance(): void
    {
        $v1 = Vector::fromArray([1, 2, 3]);
        $v2 = +$v1;

        $this->assertNotSame($v1, $v2);

        $v2->set(0, 99);
        $this->assertSame(1.0, $v1->get(0));
    }

    /**
     * Test unary + on an empty Vector.
     */
    public function testUnaryPlusEmpty(): void
    {
        $v1 = Vector::fromArray([]);
        $v2 = +$v1;

        $this->assertSame([], $v2->toArray());
    }

    #endregion

    #region Operator - (negate) tests.

    /**
     * Test that unary - is equivalent to neg(): negates every element.
     */
    public function testUnaryMinus(): void
    {
        $v = Vector::fromArray([1, -2, 3]);
        $negated = -$v;

        $this->assertSame([-1.0, 2.0, -3.0], $negated->toArray());
        $this->assertEquals($v->neg(), $negated);
    }

    /**
     * Test that unary - does not modify the original.
     */
    public function testUnaryMinusDoesNotMutate(): void
    {
        $v = Vector::fromArray([1, -2, 3]);

        $result = -$v;

        $this->assertSame([1.0, -2.0, 3.0], $v->toArray());
    }

    /**
     * Test double negation returns the original value.
     */
    public function testDoubleUnaryMinus(): void
    {
        $v = Vector::fromArray([1, -2, 3]);
        $result = -(-$v);

        $this->assertSame([1.0, -2.0, 3.0], $result->toArray());
    }

    #endregion
}
