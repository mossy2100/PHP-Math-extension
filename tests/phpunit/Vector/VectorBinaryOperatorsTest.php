<?php

declare(strict_types=1);

namespace OceanMoon\Math\Tests\Vector;

use DomainException;
use LengthException;
use OceanMoon\Core\Exceptions\ArithmeticException;
use OceanMoon\Math\Matrix;
use OceanMoon\Math\Vector;
use OverflowException;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\TestCase;
use TypeError;

/**
 * Tests for Vector's binary operator overloads (+, -, *, /), only available in the extension -
 * see docs/Vector.md in this repo. Each is equivalent to calling the same-named method directly,
 * and throws the same exceptions under the same conditions - except that + and - only accept
 * another Vector (no scalar form exists), * accepts a scalar (either side, commutative) or a
 * Matrix (Vector on the left only), and / accepts a scalar divisor (Vector on the left) or a
 * scalar dividend (Vector on the right, element-wise, with no equivalent named method).
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
        $v1 = Vector::fromArray([1, 2, 3]);
        $v2 = Vector::fromArray([4, 5, 6]);
        $result = $v1 + $v2;

        $this->assertSame([5.0, 7.0, 9.0], $result->toArray());
        $this->assertEquals($v1->add($v2), $result);
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
        $v1 = Vector::fromArray([1, 2, 3]);
        $v2 = Vector::fromArray([4, 5, 6]);

        $result = $v1 + $v2;

        $this->assertSame([1.0, 2.0, 3.0], $v1->toArray());
        $this->assertSame([4.0, 5.0, 6.0], $v2->toArray());
    }

    /**
     * Test + throws OverflowException when the result overflows, matching add().
     */
    public function testAddOverflowThrows(): void
    {
        $v = Vector::fromArray([1e308]);

        $this->expectException(OverflowException::class);
        $this->expectExceptionMessage('Cannot add the given Vectors. The result would overflow.');
        $result = $v + $v;
    }

    #endregion

    #region Operator - (subtract) tests.

    /**
     * Test Vector - Vector.
     */
    public function testSubVectorMinusVector(): void
    {
        $v1 = Vector::fromArray([4, 5, 6]);
        $v2 = Vector::fromArray([1, 2, 3]);
        $result = $v1 - $v2;

        $this->assertSame([3.0, 3.0, 3.0], $result->toArray());
        $this->assertEquals($v1->sub($v2), $result);
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
        $v1 = Vector::fromArray([4, 5, 6]);
        $v2 = Vector::fromArray([1, 2, 3]);

        $result = $v1 - $v2;

        $this->assertSame([4.0, 5.0, 6.0], $v1->toArray());
        $this->assertSame([1.0, 2.0, 3.0], $v2->toArray());
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
        $m = Matrix::fromArray([
            [1, 0],
            [0, 1],
        ]);
        $v = Vector::fromArray([1, 2]);
        $result = $v * $m;

        $this->assertSame([1.0, 2.0], $result->toArray());
        $this->assertEquals($v->mul($m), $result);
    }

    /**
     * Test Vector * Matrix with incompatible dimensions throws the same exception as mul().
     */
    public function testMulVectorTimesMatrixIncompatibleDimensionsThrows(): void
    {
        $m = Matrix::fromArray([
            [1, 0, 0],
            [0, 1, 0],
            [0, 0, 1],
        ]);
        $v = Vector::fromArray([1, 2]);

        $this->expectException(LengthException::class);
        $result = $v * $m;
    }

    /**
     * Test Vector * Vector is the Hadamard (element-wise) product, matching mul() with a Vector.
     */
    public function testMulVectorTimesVector(): void
    {
        $v1 = Vector::fromArray([1, 2, 3]);
        $v2 = Vector::fromArray([4, 5, 6]);
        $result = $v1 * $v2;

        $this->assertSame([4.0, 10.0, 18.0], $result->toArray());
        $this->assertEquals($v1->mul($v2), $result);
    }

    /**
     * Test Vector * Vector with different counts throws the same exception as mul().
     */
    public function testMulVectorTimesVectorDifferentCountsThrows(): void
    {
        $v1 = Vector::fromArray([1, 2, 3]);
        $v2 = Vector::fromArray([4, 5]);

        $this->expectException(LengthException::class);
        $this->expectExceptionMessage('Cannot multiply by Vector of incorrect count: 2. Expected 3.');
        $result = $v1 * $v2;
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

    /**
     * Test * throws OverflowException when the result overflows, matching mul().
     */
    public function testMulOverflowThrows(): void
    {
        $v = Vector::fromArray([1e200]);

        $this->expectException(OverflowException::class);
        $this->expectExceptionMessage('Cannot multiply the given Vector by 1.0E+200. The result would overflow.');
        $result = 1e200 * $v;
    }

    /**
     * Test multiplication by a non-finite scalar throws DomainException, matching mul().
     */
    public function testMulByNonFiniteThrows(): void
    {
        $v = new Vector(0);

        $this->expectException(DomainException::class);
        $this->expectExceptionMessage('Invalid scalar: INF. Must be finite.');
        $result = $v * INF;
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
     * Test int|float / Vector: not commutative with Vector / int|float - element-wise, divides the
     * scalar by each element in turn, equivalent to $x * $v->reciprocal().
     */
    public function testDivScalarOverVector(): void
    {
        $v = Vector::fromArray([1, 2, 4]);
        $result = 8 / $v;

        $this->assertSame([8.0, 4.0, 2.0], $result->toArray());
        $this->assertEquals(8 * $v->reciprocal(), $result);
    }

    /**
     * Test int|float / Vector with a zero element throws ArithmeticException, matching
     * reciprocal().
     */
    public function testDivScalarOverVectorZeroElementThrows(): void
    {
        $v = Vector::fromArray([1, 0, 4]);

        $this->expectException(ArithmeticException::class);
        $result = 8 / $v;
    }

    /**
     * Test Vector / Vector is Hadamard (element-wise) division, matching div() with a Vector.
     */
    public function testDivVectorOverVector(): void
    {
        $v1 = Vector::fromArray([2, 9, 6]);
        $v2 = Vector::fromArray([1, 3, 4]);
        $result = $v1 / $v2;

        $this->assertSame([2.0, 3.0, 1.5], $result->toArray());
        $this->assertEquals($v1->div($v2), $result);
    }

    /**
     * Test Vector / Vector with a zero element in the divisor throws the same exception as div().
     */
    public function testDivVectorOverVectorZeroElementThrows(): void
    {
        $v1 = Vector::fromArray([2, 4, 6]);
        $v2 = Vector::fromArray([1, 0, 3]);

        $this->expectException(ArithmeticException::class);
        $this->expectExceptionMessage('Cannot divide by zero at index 1.');
        $result = $v1 / $v2;
    }

    /**
     * Test Vector / Vector with different counts throws the same exception as div().
     */
    public function testDivVectorOverVectorDifferentCountsThrows(): void
    {
        $v1 = Vector::fromArray([2, 4, 6]);
        $v2 = Vector::fromArray([1, 2]);

        $this->expectException(LengthException::class);
        $this->expectExceptionMessage('Cannot divide by Vector of incorrect count: 2. Expected 3.');
        $result = $v1 / $v2;
    }

    /**
     * Test Vector / Matrix is unsupported: ambiguous between M⁻¹*V and V*M⁻¹.
     */
    public function testDivVectorOverMatrixThrows(): void
    {
        $v = Vector::fromArray([1, 2]);
        $m = Matrix::fromArray([
            [1, 0],
            [0, 1],
        ]);

        $this->expectException(TypeError::class);
        $result = $v / $m; // @phpstan-ignore binaryOp.invalid
    }

    /**
     * Test division by zero throws ArithmeticException, matching div().
     */
    public function testDivByZeroThrows(): void
    {
        $v = Vector::fromArray([2, 4, 6]);

        $this->expectException(ArithmeticException::class);
        $v / 0; // @phpstan-ignore binaryOp.invalid
    }

    /**
     * Test division by a non-finite scalar throws DomainException, matching div().
     */
    public function testDivByNonFiniteThrows(): void
    {
        $v = Vector::fromArray([2, 4, 6]);

        $this->expectException(DomainException::class);
        $this->expectExceptionMessage('Invalid scalar: INF. Must be finite.');
        $result = $v / INF;
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

    /**
     * Test scalar / Vector throws OverflowException when the result overflows.
     */
    public function testScalarDivOverflowThrows(): void
    {
        $v = Vector::fromArray([1e-300]);

        $this->expectException(OverflowException::class);
        $this->expectExceptionMessage('Cannot divide 1.0E+300 by the given Vector. The result would overflow.');
        $result = 1e300 / $v;
    }

    #endregion
}
