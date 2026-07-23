<?php

declare(strict_types=1);

namespace OceanMoon\Math\Tests\Matrix;

use OceanMoon\Core\Exceptions\ArithmeticException;
use OceanMoon\Math\Matrix;
use OceanMoon\Math\Vector;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\TestCase;
use TypeError;

/**
 * Tests for Matrix's binary operator overloads (+, -, *, /, **), only available in the extension --
 * see docs/Matrix.md in this repo. Each is equivalent to calling the same-named method directly,
 * and throws the same exceptions under the same conditions -- except that + and - only accept
 * another Matrix (no scalar form exists), * accepts a scalar (either side, commutative), a Matrix,
 * or a Vector (Matrix on the left only, treating the Vector as a column matrix), / only accepts a
 * scalar divisor (Matrix on the left only), and ** only accepts an int exponent (Matrix on the left
 * only).
 */
#[CoversClass(Matrix::class)]
#[CoversClass(Vector::class)]
class MatrixBinaryOperatorsTest extends TestCase
{
    #region Operator + (add) tests.

    /**
     * Test Matrix + Matrix.
     */
    public function testAddMatrixPlusMatrix(): void
    {
        $A = Matrix::fromArray([[1, 2], [3, 4]]);
        $B = Matrix::fromArray([[5, 6], [7, 8]]);
        $result = $A + $B;

        $this->assertSame([[6.0, 8.0], [10.0, 12.0]], $result->toArray());
        $this->assertEquals($A->add($B), $result);
    }

    /**
     * Test Matrix + int is unsupported: there's no scalar addition for Matrix.
     */
    public function testAddMatrixPlusScalarThrows(): void
    {
        $A = Matrix::fromArray([[1, 2], [3, 4]]);

        $this->expectException(TypeError::class);
        $result = $A + 1; // @phpstan-ignore binaryOp.invalid
    }

    /**
     * Test + does not modify either operand (Matrix is mutable, so this matters).
     */
    public function testAddDoesNotMutate(): void
    {
        $A = Matrix::fromArray([[1, 2], [3, 4]]);
        $B = Matrix::fromArray([[5, 6], [7, 8]]);

        $result = $A + $B;

        $this->assertSame([[1.0, 2.0], [3.0, 4.0]], $A->toArray());
        $this->assertSame([[5.0, 6.0], [7.0, 8.0]], $B->toArray());
    }

    #endregion

    #region Operator - (subtract) tests.

    /**
     * Test Matrix - Matrix.
     */
    public function testSubMatrixMinusMatrix(): void
    {
        $A = Matrix::fromArray([[5, 6], [7, 8]]);
        $B = Matrix::fromArray([[1, 2], [3, 4]]);
        $result = $A - $B;

        $this->assertSame([[4.0, 4.0], [4.0, 4.0]], $result->toArray());
        $this->assertEquals($A->sub($B), $result);
    }

    /**
     * Test Matrix - int is unsupported: there's no scalar subtraction for Matrix.
     */
    public function testSubMatrixMinusScalarThrows(): void
    {
        $A = Matrix::fromArray([[5, 6], [7, 8]]);

        $this->expectException(TypeError::class);
        $result = $A - 1; // @phpstan-ignore binaryOp.invalid
    }

    /**
     * Test - does not modify either operand.
     */
    public function testSubDoesNotMutate(): void
    {
        $A = Matrix::fromArray([[5, 6], [7, 8]]);
        $B = Matrix::fromArray([[1, 2], [3, 4]]);

        $result = $A - $B;

        $this->assertSame([[5.0, 6.0], [7.0, 8.0]], $A->toArray());
        $this->assertSame([[1.0, 2.0], [3.0, 4.0]], $B->toArray());
    }

    #endregion

    #region Operator * (multiply) tests.

    /**
     * Test Matrix * int and Matrix * float (element-wise scalar multiplication).
     */
    public function testMulMatrixTimesScalar(): void
    {
        $A = Matrix::fromArray([[1, 2], [3, 4]]);

        $this->assertSame([[2.0, 4.0], [6.0, 8.0]], ($A * 2)->toArray());
        $this->assertEquals($A->mul(2.0), $A * 2);
    }

    /**
     * Test int|float * Matrix gives the same result as Matrix * int|float (commutative).
     */
    public function testMulScalarTimesMatrix(): void
    {
        $A = Matrix::fromArray([[1, 2], [3, 4]]);

        $this->assertEquals($A * 2, 2 * $A);
    }

    /**
     * Test Matrix * Matrix: standard matrix multiplication.
     */
    public function testMulMatrixTimesMatrix(): void
    {
        $A = Matrix::fromArray([[1, 2], [3, 4]]);
        $B = Matrix::fromArray([[5, 6], [7, 8]]);
        $result = $A * $B;

        $this->assertSame([[19.0, 22.0], [43.0, 50.0]], $result->toArray());
        $this->assertEquals($A->mul($B), $result);
    }

    /**
     * Test Matrix * Matrix with incompatible dimensions throws the same exception as mul().
     */
    public function testMulMatrixTimesMatrixIncompatibleDimensionsThrows(): void
    {
        $A = Matrix::fromArray([[1, 2], [3, 4]]);
        $B = Matrix::fromArray([[1, 2, 3]]);

        $this->expectException(\LengthException::class);
        $result = $A * $B;
    }

    /**
     * Test Matrix * Vector: the Vector is treated as a single-column matrix, result is a Vector --
     * equivalent to mulVector(), not mul().
     */
    public function testMulMatrixTimesVector(): void
    {
        $A = Matrix::fromArray([[1, 2], [3, 4]]);
        $v = Vector::fromArray([1, 2]);
        $result = $A * $v;

        $this->assertSame([5.0, 11.0], $result->toArray());
        $this->assertEquals($A->mulVector($v), $result);
    }

    /**
     * Test * does not modify either operand.
     */
    public function testMulDoesNotMutate(): void
    {
        $A = Matrix::fromArray([[1, 2], [3, 4]]);
        $B = Matrix::fromArray([[5, 6], [7, 8]]);

        $result = $A * $B;

        $this->assertSame([[1.0, 2.0], [3.0, 4.0]], $A->toArray());
        $this->assertSame([[5.0, 6.0], [7.0, 8.0]], $B->toArray());
    }

    #endregion

    #region Operator / (divide) tests.

    /**
     * Test Matrix / int and Matrix / float.
     */
    public function testDivMatrixOverScalar(): void
    {
        $A = Matrix::fromArray([[2, 4], [6, 8]]);

        $this->assertSame([[1.0, 2.0], [3.0, 4.0]], ($A / 2)->toArray());
        $this->assertEquals($A->div(2.0), $A / 2);
    }

    /**
     * Test int|float / Matrix is unsupported: ambiguous between x*A⁻¹ and element-wise division.
     */
    public function testDivScalarOverMatrixThrows(): void
    {
        $A = Matrix::fromArray([[2, 4], [6, 8]]);

        $this->expectException(TypeError::class);
        $result = 2 / $A; // @phpstan-ignore binaryOp.invalid
    }

    /**
     * Test Matrix / Matrix is unsupported: ambiguous between A*B⁻¹, B⁻¹*A, and Hadamard division.
     */
    public function testDivMatrixOverMatrixThrows(): void
    {
        $A = Matrix::fromArray([[2, 4], [6, 8]]);
        $B = Matrix::fromArray([[1, 0], [0, 1]]);

        $this->expectException(TypeError::class);
        $result = $A / $B; // @phpstan-ignore binaryOp.invalid
    }

    /**
     * Test division by zero throws ArithmeticException, matching div().
     */
    public function testDivByZeroThrows(): void
    {
        $A = Matrix::fromArray([[2, 4], [6, 8]]);

        $this->expectException(ArithmeticException::class);
        $A / 0; // @phpstan-ignore binaryOp.invalid
    }

    /**
     * Test / does not modify the operand.
     */
    public function testDivDoesNotMutate(): void
    {
        $A = Matrix::fromArray([[2, 4], [6, 8]]);

        $result = $A / 2;

        $this->assertSame([[2.0, 4.0], [6.0, 8.0]], $A->toArray());
    }

    #endregion

    #region Operator ** (power) tests.

    /**
     * Test Matrix ** int.
     */
    public function testPowMatrixToInt(): void
    {
        $A = Matrix::fromArray([[1, 1], [0, 1]]);
        $result = $A ** 2;

        $this->assertSame([[1.0, 2.0], [0.0, 1.0]], $result->toArray());
        $this->assertEquals($A->pow(2), $result);
    }

    /**
     * Test Matrix ** negative int: equivalent to inverting first.
     */
    public function testPowMatrixToNegativeInt(): void
    {
        $A = Matrix::fromArray([[1, 2], [3, 4]]);
        $result = $A ** -1;

        $this->assertEquals($A->inv(), $result);
    }

    /**
     * Test int ** Matrix is unsupported: a scalar raised to a matrix power isn't a standard
     * operation in linear algebra.
     */
    public function testPowIntToMatrixThrows(): void
    {
        $A = Matrix::fromArray([[1, 2], [3, 4]]);

        $this->expectException(TypeError::class);
        $result = 2 ** $A; // @phpstan-ignore binaryOp.invalid
    }

    /**
     * Test Matrix ** Matrix is unsupported.
     */
    public function testPowMatrixToMatrixThrows(): void
    {
        $A = Matrix::fromArray([[1, 2], [3, 4]]);
        $B = Matrix::fromArray([[1, 0], [0, 1]]);

        $this->expectException(TypeError::class);
        $result = $A ** $B;
    }

    /**
     * Test ** does not modify the operand.
     */
    public function testPowDoesNotMutate(): void
    {
        $A = Matrix::fromArray([[1, 1], [0, 1]]);

        $result = $A ** 2;

        $this->assertSame([[1.0, 1.0], [0.0, 1.0]], $A->toArray());
    }

    #endregion

    #region Inverting with A ** -1 tests.

    /**
     * Test that $A ** -1 and $A->inv() give the same result.
     */
    public function testInvertingEquivalence(): void
    {
        $A = Matrix::fromArray([[1, 2], [3, 4]]);

        $inv = $A->inv();
        $viaPower = $A ** -1;

        $this->assertEquals($inv, $viaPower);
    }

    /**
     * Test that inverting via exponentiation throws ArithmeticException for a zero-determinant
     * Matrix, matching inv().
     */
    public function testInvertingSingularByPowThrows(): void
    {
        $singular = Matrix::fromArray([[1, 2], [2, 4]]);

        $this->expectException(ArithmeticException::class);
        $result = $singular ** -1;
    }

    /**
     * Test that inverting via exponentiation throws DomainException for a non-square Matrix,
     * matching inv().
     */
    public function testInvertingNonSquareByPowThrows(): void
    {
        $nonSquare = Matrix::fromArray([[1, 2, 3], [4, 5, 6]]);

        $this->expectException(\DomainException::class);
        $result = $nonSquare ** -1;
    }

    #endregion
}
