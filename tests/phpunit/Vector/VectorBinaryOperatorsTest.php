<?php

declare(strict_types=1);

namespace OceanMoon\Math\Tests\Vector;

use OceanMoon\Math\Matrix;
use OceanMoon\Math\Vector;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\TestCase;
use TypeError;

/**
 * Tests for Vector's binary operator overloads (+, -, *, /), only available in the extension --
 * see docs/Vector.md in this repo. Each is equivalent to calling the same-named method directly,
 * and throws the same exceptions under the same conditions -- except that + and - only accept
 * another Vector (no scalar form exists), * accepts a scalar (either side, commutative) or a
 * Matrix (Vector on the left only), and / only accepts a scalar divisor (Vector on the left only).
 */
#[CoversClass(Vector::class)]
#[CoversClass(Matrix::class)]
class VectorBinaryOperatorsTest extends TestCase
{
    #region Operator + (add) tests.

    /**
     * Test Vector + Vector.
     */
    public function testAddVectorPlusVector(): void
    {
        $v = Vector::fromArray([1, 2, 3]);
        $u = Vector::fromArray([4, 5, 6]);
        $result = $v + $u;

        $this->assertSame([5.0, 7.0, 9.0], $result->toArray());
        $this->assertEquals($v->add($u), $result);
    }

    /**
     * Test Vector + int is unsupported: there's no scalar addition for Vector.
     */
    public function testAddVectorPlusScalarThrows(): void
    {
        $v = Vector::fromArray([1, 2, 3]);

        $this->expectException(TypeError::class);
        $result = $v + 1; // @phpstan-ignore binaryOp.invalid
    }

    /**
     * Test + does not modify either operand (Vector is mutable, so this matters).
     */
    public function testAddDoesNotMutate(): void
    {
        $v = Vector::fromArray([1, 2, 3]);
        $u = Vector::fromArray([4, 5, 6]);

        $result = $v + $u;

        $this->assertSame([1.0, 2.0, 3.0], $v->toArray());
        $this->assertSame([4.0, 5.0, 6.0], $u->toArray());
    }

    #endregion

    #region Operator - (subtract) tests.

    /**
     * Test Vector - Vector.
     */
    public function testSubVectorMinusVector(): void
    {
        $v = Vector::fromArray([4, 5, 6]);
        $u = Vector::fromArray([1, 2, 3]);
        $result = $v - $u;

        $this->assertSame([3.0, 3.0, 3.0], $result->toArray());
        $this->assertEquals($v->sub($u), $result);
    }

    /**
     * Test Vector - int is unsupported: there's no scalar subtraction for Vector.
     */
    public function testSubVectorMinusScalarThrows(): void
    {
        $v = Vector::fromArray([4, 5, 6]);

        $this->expectException(TypeError::class);
        $result = $v - 1; // @phpstan-ignore binaryOp.invalid
    }

    /**
     * Test - does not modify either operand.
     */
    public function testSubDoesNotMutate(): void
    {
        $v = Vector::fromArray([4, 5, 6]);
        $u = Vector::fromArray([1, 2, 3]);

        $result = $v - $u;

        $this->assertSame([4.0, 5.0, 6.0], $v->toArray());
        $this->assertSame([1.0, 2.0, 3.0], $u->toArray());
    }

    #endregion

    #region Operator * (multiply) tests.

    /**
     * Test Vector * int and Vector * float (scalar multiplication).
     */
    public function testMulVectorTimesScalar(): void
    {
        $v = Vector::fromArray([1, 2, 3]);

        $this->assertSame([2.0, 4.0, 6.0], ($v * 2)->toArray());
        $this->assertSame([2.5, 5.0, 7.5], ($v * 2.5)->toArray());
        $this->assertEquals($v->mul(2.0), $v * 2);
    }

    /**
     * Test int|float * Vector gives the same result as Vector * int|float (commutative).
     */
    public function testMulScalarTimesVector(): void
    {
        $v = Vector::fromArray([1, 2, 3]);

        $this->assertEquals($v * 2, 2 * $v);
        $this->assertEquals($v * 2.5, 2.5 * $v);
    }

    /**
     * Test Vector * Matrix: this Vector is treated as a 1xn row matrix.
     */
    public function testMulVectorTimesMatrix(): void
    {
        $A = Matrix::fromArray([[1, 0], [0, 1]]);
        $u = Vector::fromArray([1, 2]);
        $result = $u * $A;

        $this->assertSame([1.0, 2.0], $result->toArray());
        $this->assertEquals($u->mul($A), $result);
    }

    /**
     * Test Vector * Matrix with incompatible dimensions throws the same exception as mul().
     */
    public function testMulVectorTimesMatrixIncompatibleDimensionsThrows(): void
    {
        $A = Matrix::fromArray([[1, 0, 0], [0, 1, 0], [0, 0, 1]]);
        $u = Vector::fromArray([1, 2]);

        $this->expectException(\LengthException::class);
        $result = $u * $A;
    }

    /**
     * Test Vector * Vector is unsupported: multiple ways to multiply two vectors exist
     * (dot/cross/hadamardMul/outer), so no single operator meaning is unambiguous.
     */
    public function testMulVectorTimesVectorThrows(): void
    {
        $v = Vector::fromArray([1, 2, 3]);
        $u = Vector::fromArray([4, 5, 6]);

        $this->expectException(TypeError::class);
        $result = $v * $u; // @phpstan-ignore binaryOp.invalid
    }

    /**
     * Test * does not modify either operand.
     */
    public function testMulDoesNotMutate(): void
    {
        $v = Vector::fromArray([1, 2, 3]);

        $result = $v * 2;

        $this->assertSame([1.0, 2.0, 3.0], $v->toArray());
    }

    #endregion

    #region Operator / (divide) tests.

    /**
     * Test Vector / int and Vector / float.
     */
    public function testDivVectorOverScalar(): void
    {
        $v = Vector::fromArray([2, 4, 6]);

        $this->assertSame([1.0, 2.0, 3.0], ($v / 2)->toArray());
        $this->assertEquals($v->div(2.0), $v / 2);
    }

    /**
     * Test int|float / Vector is unsupported: Vector has no multiplicative inverse for a scalar to
     * be divided by.
     */
    public function testDivScalarOverVectorThrows(): void
    {
        $v = Vector::fromArray([2, 4, 6]);

        $this->expectException(TypeError::class);
        $result = 2 / $v; // @phpstan-ignore binaryOp.invalid
    }

    /**
     * Test Vector / Vector is unsupported: use hadamardDiv() for element-wise division instead.
     */
    public function testDivVectorOverVectorThrows(): void
    {
        $v = Vector::fromArray([2, 4, 6]);
        $u = Vector::fromArray([1, 2, 3]);

        $this->expectException(TypeError::class);
        $result = $v / $u; // @phpstan-ignore binaryOp.invalid
    }

    /**
     * Test Vector / Matrix is unsupported: ambiguous between M⁻¹*V and V*M⁻¹.
     */
    public function testDivVectorOverMatrixThrows(): void
    {
        $v = Vector::fromArray([1, 2]);
        $A = Matrix::fromArray([[1, 0], [0, 1]]);

        $this->expectException(TypeError::class);
        $result = $v / $A; // @phpstan-ignore binaryOp.invalid
    }

    /**
     * Test division by zero throws ArithmeticException, matching div().
     */
    public function testDivByZeroThrows(): void
    {
        $v = Vector::fromArray([2, 4, 6]);

        $this->expectException(\OceanMoon\Core\Exceptions\ArithmeticException::class);
        $v / 0; // @phpstan-ignore binaryOp.invalid
    }

    /**
     * Test / does not modify the operand.
     */
    public function testDivDoesNotMutate(): void
    {
        $v = Vector::fromArray([2, 4, 6]);

        $result = $v / 2;

        $this->assertSame([2.0, 4.0, 6.0], $v->toArray());
    }

    #endregion
}
