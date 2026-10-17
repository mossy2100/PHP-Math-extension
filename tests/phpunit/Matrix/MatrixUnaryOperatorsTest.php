<?php

declare(strict_types=1);

namespace OceanMoon\Math\Tests\Matrix;

use OceanMoon\Math\Matrix;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\TestCase;
use TypeError;

/**
 * Tests for Matrix's unary operator overloads (+, -), only available in the extension - see
 * docs/Matrix.md in this repo. PHP has no dedicated opcode for unary +/-; both lower to `$m * 1`/
 * `$m * -1`, so these exercise the ZEND_MUL case of the do_operation handler indirectly. Also checks
 * that ~, ++ and -- are unsupported, throwing TypeError.
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
        $m1 = Matrix::fromArray([
            [1, 2],
            [3, 4],
        ]);
        $m2 = +$m1;

        $this->assertSame([
            [1.0, 2.0],
            [3.0, 4.0],
        ], $m2->toArray());
    }

    /**
     * Test that unary + returns a new instance, not $m1 itself (Matrix is mutable, so this matters).
     */
    public function testUnaryPlusReturnsNewInstance(): void
    {
        $m1 = Matrix::fromArray([
            [1, 2],
            [3, 4],
        ]);
        $m2 = +$m1;

        $this->assertNotSame($m1, $m2);

        $m2->set(0, 0, 99);
        $this->assertSame(1.0, $m1->get(0, 0));
    }

    /**
     * Test that unary + deep-clones the row Vectors too, not just the Matrix object - mutating a
     * row of the clone via ArrayAccess (e.g. $m2[0][1] = ...) must not affect $m1's row, which
     * would happen if the clone's $data array held the same row Vector instances as the original.
     */
    public function testUnaryPlusClonesRows(): void
    {
        $m1 = Matrix::fromArray([
            [1, 2],
            [3, 4],
        ]);
        $m2 = +$m1;

        $this->assertNotSame($m1[0], $m2[0]);

        $m2[0][1] = -1;
        $this->assertSame(2.0, $m1->get(0, 1));
    }

    #endregion

    #region Operator - (negate) tests.

    /**
     * Test that unary - is equivalent to neg(): negates every element.
     */
    public function testUnaryMinus(): void
    {
        $m = Matrix::fromArray([
            [1, -2],
            [3, -4],
        ]);
        $negated = -$m;

        $this->assertSame([
            [-1.0, 2.0],
            [-3.0, 4.0],
        ], $negated->toArray());
        $this->assertEquals($m->neg(), $negated);
    }

    /**
     * Test that unary - does not modify the original.
     */
    public function testUnaryMinusDoesNotMutate(): void
    {
        $m = Matrix::fromArray([
            [1, -2],
            [3, -4],
        ]);

        $result = -$m;

        $this->assertSame([
            [1.0, -2.0],
            [3.0, -4.0],
        ], $m->toArray());
    }

    /**
     * Test double negation returns the original value.
     */
    public function testDoubleUnaryMinus(): void
    {
        $m = Matrix::fromArray([
            [1, -2],
            [3, -4],
        ]);
        $result = -(-$m);

        $this->assertSame([
            [1.0, -2.0],
            [3.0, -4.0],
        ], $result->toArray());
    }

    #endregion

    #region Operator ~ tests.

    /**
     * Test that ~ is unsupported for Matrix, and throws rather than crashing.
     */
    public function testBitwiseNotThrows(): void
    {
        $m = Matrix::fromArray([[1, 2], [3, 4]]);

        $this->expectException(TypeError::class);
        $this->expectExceptionMessage('Cannot perform bitwise not on OceanMoon\Math\Matrix');

        $result = ~$m;
    }

    #endregion

    #region Operator ++ and -- tests.

    /**
     * Test that ++ is unsupported for Matrix, since Matrix + int is.
     */
    public function testIncrementThrows(): void
    {
        $m = Matrix::fromArray([[1, 2], [3, 4]]);

        $this->expectException(TypeError::class);
        $this->expectExceptionMessage('Cannot increment OceanMoon\Math\Matrix');

        $m++;
    }

    /**
     * Test that -- is unsupported for Matrix, since Matrix - int is.
     */
    public function testDecrementThrows(): void
    {
        $m = Matrix::fromArray([[1, 2], [3, 4]]);

        $this->expectException(TypeError::class);
        $this->expectExceptionMessage('Cannot decrement OceanMoon\Math\Matrix');

        $m--;
    }

    #endregion
}
