<?php

declare(strict_types=1);

namespace OceanMoon\Math\Tests\Matrix;

use OceanMoon\Math\Matrix;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\TestCase;

/**
 * Tests for Matrix's unary operator overloads (+, -), only available in the extension -- see
 * docs/Matrix.md in this repo. PHP has no dedicated opcode for unary +/-; both lower to `$matA * 1`/
 * `$matA * -1`, so these exercise the ZEND_MUL case of the do_operation handler indirectly.
 */
#[CoversClass(Matrix::class)]
class MatrixUnaryOperatorsTest extends TestCase
{
    #region Operator + (identity) tests.

    /**
     * Test that unary + returns an equal Matrix with the same elements.
     */
    public function testUnaryPlus(): void
    {
        $matA = Matrix::fromArray([
            [1, 2],
            [3, 4],
        ]);
        $matB = +$matA;

        $this->assertSame([
            [1.0, 2.0],
            [3.0, 4.0],
        ], $matB->toArray());
    }

    /**
     * Test that unary + returns a new instance, not $matA itself (Matrix is mutable, so this matters).
     */
    public function testUnaryPlusReturnsNewInstance(): void
    {
        $matA = Matrix::fromArray([
            [1, 2],
            [3, 4],
        ]);
        $matB = +$matA;

        $this->assertNotSame($matA, $matB);

        $matB->set(0, 0, 99);
        $this->assertSame(1.0, $matA->get(0, 0));
    }

    #endregion

    #region Operator - (negate) tests.

    /**
     * Test that unary - is equivalent to neg(): negates every element.
     */
    public function testUnaryMinus(): void
    {
        $matA = Matrix::fromArray([
            [1, -2],
            [3, -4],
        ]);
        $negated = -$matA;

        $this->assertSame([
            [-1.0, 2.0],
            [-3.0, 4.0],
        ], $negated->toArray());
        $this->assertEquals($matA->neg(), $negated);
    }

    /**
     * Test that unary - does not modify the original.
     */
    public function testUnaryMinusDoesNotMutate(): void
    {
        $matA = Matrix::fromArray([
            [1, -2],
            [3, -4],
        ]);

        $result = -$matA;

        $this->assertSame([
            [1.0, -2.0],
            [3.0, -4.0],
        ], $matA->toArray());
    }

    /**
     * Test double negation returns the original value.
     */
    public function testDoubleUnaryMinus(): void
    {
        $matA = Matrix::fromArray([
            [1, -2],
            [3, -4],
        ]);
        $result = -(-$matA);

        $this->assertSame([
            [1.0, -2.0],
            [3.0, -4.0],
        ], $result->toArray());
    }

    #endregion
}
