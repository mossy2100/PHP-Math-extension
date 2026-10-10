<?php

declare(strict_types=1);

namespace OceanMoon\Math\Tests\Matrix;

use DomainException;
use LengthException;
use OceanMoon\Core\Exceptions\ArithmeticException;
use OceanMoon\Math\Matrix;
use OceanMoon\Math\Vector;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\TestCase;
use TypeError;

/**
 * Tests for Matrix's binary operator overloads (+, -, *, /, **), only available in the extension -
 * see docs/Matrix.md in this repo. Each is equivalent to calling the same-named method directly,
 * and throws the same exceptions under the same conditions - except that + and - only accept
 * another Matrix (no scalar form exists), * accepts a scalar (either side, commutative), a Matrix,
 * or a Vector (Matrix on the left only, treating the Vector as a column matrix), / accepts a scalar
 * divisor (Matrix on the left) or a scalar dividend (Matrix on the right, element-wise, with no
 * equivalent named method), and ** only accepts an int exponent (Matrix on the left only).
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
        $m1 = Matrix::fromArray([
            [1, 2],
            [3, 4],
        ]);
        $m2 = Matrix::fromArray([
            [5, 6],
            [7, 8],
        ]);
        $result = $m1 + $m2;

        $this->assertSame([
            [6.0, 8.0],
            [10.0, 12.0],
        ], $result->toArray());
        $this->assertEquals($m1->add($m2), $result);
    }

    /**
     * Test Matrix + int is unsupported: there's no scalar addition for Matrix.
     */
    public function testAddMatrixPlusScalarThrows(): void
    {
        $m = Matrix::fromArray([
            [1, 2],
            [3, 4],
        ]);

        $this->expectException(TypeError::class);
        $result = $m + 1; // @phpstan-ignore binaryOp.invalid
    }

    /**
     * Test + does not modify either operand (Matrix is mutable, so this matters).
     */
    public function testAddDoesNotMutate(): void
    {
        $m1 = Matrix::fromArray([
            [1, 2],
            [3, 4],
        ]);
        $m2 = Matrix::fromArray([
            [5, 6],
            [7, 8],
        ]);

        $result = $m1 + $m2;

        $this->assertSame([
            [1.0, 2.0],
            [3.0, 4.0],
        ], $m1->toArray());
        $this->assertSame([
            [5.0, 6.0],
            [7.0, 8.0],
        ], $m2->toArray());
    }

    #endregion

    #region Operator - (subtract) tests.

    /**
     * Test Matrix - Matrix.
     */
    public function testSubMatrixMinusMatrix(): void
    {
        $m1 = Matrix::fromArray([
            [5, 6],
            [7, 8],
        ]);
        $m2 = Matrix::fromArray([
            [1, 2],
            [3, 4],
        ]);
        $result = $m1 - $m2;

        $this->assertSame([
            [4.0, 4.0],
            [4.0, 4.0],
        ], $result->toArray());
        $this->assertEquals($m1->sub($m2), $result);
    }

    /**
     * Test Matrix - int is unsupported: there's no scalar subtraction for Matrix.
     */
    public function testSubMatrixMinusScalarThrows(): void
    {
        $m = Matrix::fromArray([
            [5, 6],
            [7, 8],
        ]);

        $this->expectException(TypeError::class);
        $result = $m - 1; // @phpstan-ignore binaryOp.invalid
    }

    /**
     * Test - does not modify either operand.
     */
    public function testSubDoesNotMutate(): void
    {
        $m1 = Matrix::fromArray([
            [5, 6],
            [7, 8],
        ]);
        $m2 = Matrix::fromArray([
            [1, 2],
            [3, 4],
        ]);

        $result = $m1 - $m2;

        $this->assertSame([
            [5.0, 6.0],
            [7.0, 8.0],
        ], $m1->toArray());
        $this->assertSame([
            [1.0, 2.0],
            [3.0, 4.0],
        ], $m2->toArray());
    }

    #endregion

    #region Operator * (multiply) tests.

    /**
     * Test Matrix * int and Matrix * float (element-wise scalar multiplication).
     */
    public function testMulMatrixTimesScalar(): void
    {
        $m = Matrix::fromArray([
            [1, 2],
            [3, 4],
        ]);

        $this->assertSame([
            [2.0, 4.0],
            [6.0, 8.0],
        ], ($m * 2)->toArray());
        $this->assertEquals($m->mul(2.0), $m * 2);
    }

    /**
     * Test int|float * Matrix gives the same result as Matrix * int|float (commutative).
     */
    public function testMulScalarTimesMatrix(): void
    {
        $m = Matrix::fromArray([
            [1, 2],
            [3, 4],
        ]);

        $this->assertEquals($m * 2, 2 * $m);
    }

    /**
     * Test Matrix * Matrix: standard matrix multiplication.
     */
    public function testMulMatrixTimesMatrix(): void
    {
        $m1 = Matrix::fromArray([
            [1, 2],
            [3, 4],
        ]);
        $m2 = Matrix::fromArray([
            [5, 6],
            [7, 8],
        ]);
        $result = $m1 * $m2;

        $this->assertSame([
            [19.0, 22.0],
            [43.0, 50.0],
        ], $result->toArray());
        $this->assertEquals($m1->mul($m2), $result);
    }

    /**
     * Test Matrix * Matrix with incompatible dimensions throws the same exception as mul().
     */
    public function testMulMatrixTimesMatrixIncompatibleDimensionsThrows(): void
    {
        $m1 = Matrix::fromArray([
            [1, 2],
            [3, 4],
        ]);
        $m2 = Matrix::fromArray([
            [1, 2, 3],
        ]);

        $this->expectException(LengthException::class);
        $result = $m1 * $m2;
    }

    /**
     * Test Matrix * Vector: the Vector is treated as a single-column matrix, result is a Vector -
     * equivalent to mulVector(), not mul().
     */
    public function testMulMatrixTimesVector(): void
    {
        $m = Matrix::fromArray([
            [1, 2],
            [3, 4],
        ]);
        $v = Vector::fromArray([1, 2]);
        $result = $m * $v;

        $this->assertSame([5.0, 11.0], $result->toArray());
        $this->assertEquals($m->mulVector($v), $result);
    }

    /**
     * Test * does not modify either operand.
     */
    public function testMulDoesNotMutate(): void
    {
        $m1 = Matrix::fromArray([
            [1, 2],
            [3, 4],
        ]);
        $m2 = Matrix::fromArray([
            [5, 6],
            [7, 8],
        ]);

        $result = $m1 * $m2;

        $this->assertSame([
            [1.0, 2.0],
            [3.0, 4.0],
        ], $m1->toArray());
        $this->assertSame([
            [5.0, 6.0],
            [7.0, 8.0],
        ], $m2->toArray());
    }

    #endregion

    #region Operator / (divide) tests.

    /**
     * Test Matrix / int and Matrix / float.
     */
    public function testDivMatrixOverScalar(): void
    {
        $m = Matrix::fromArray([
            [2, 4],
            [6, 8],
        ]);

        $this->assertSame([
            [1.0, 2.0],
            [3.0, 4.0],
        ], ($m / 2)->toArray());
        $this->assertEquals($m->div(2.0), $m / 2);
    }

    /**
     * Test int|float / Matrix: not commutative with Matrix / int|float - element-wise, divides the
     * scalar by each element in turn, equivalent to $x * $m->reciprocal(). Distinct from x*A⁻¹
     * (inverse scaling), which is $x * $m->inv() or $x * $m ** -1.
     */
    public function testDivScalarOverMatrix(): void
    {
        $m = Matrix::fromArray([
            [1, 2],
            [4, 8],
        ]);
        $result = 8 / $m;

        $this->assertSame([
            [8.0, 4.0],
            [2.0, 1.0],
        ], $result->toArray());
        $this->assertEquals(8 * $m->reciprocal(), $result);
    }

    /**
     * Test int|float / Matrix with a zero element throws ArithmeticException, matching
     * reciprocal().
     */
    public function testDivScalarOverMatrixZeroElementThrows(): void
    {
        $m = Matrix::fromArray([
            [1, 0],
            [4, 8],
        ]);

        $this->expectException(ArithmeticException::class);
        $result = 8 / $m;
    }

    /**
     * Test Matrix / Matrix is unsupported: ambiguous between A*B⁻¹, B⁻¹*A, and Hadamard division.
     */
    public function testDivMatrixOverMatrixThrows(): void
    {
        $m1 = Matrix::fromArray([
            [2, 4],
            [6, 8],
        ]);
        $m2 = Matrix::fromArray([
            [1, 0],
            [0, 1],
        ]);

        $this->expectException(TypeError::class);
        $result = $m1 / $m2; // @phpstan-ignore binaryOp.invalid
    }

    /**
     * Test division by zero throws ArithmeticException, matching div().
     */
    public function testDivByZeroThrows(): void
    {
        $m = Matrix::fromArray([
            [2, 4],
            [6, 8],
        ]);

        $this->expectException(ArithmeticException::class);
        $m / 0; // @phpstan-ignore binaryOp.invalid
    }

    /**
     * Test / does not modify the operand.
     */
    public function testDivDoesNotMutate(): void
    {
        $m = Matrix::fromArray([
            [2, 4],
            [6, 8],
        ]);

        $result = $m / 2;

        $this->assertSame([
            [2.0, 4.0],
            [6.0, 8.0],
        ], $m->toArray());
    }

    #endregion

    #region Operator ** (power) tests.

    /**
     * Test Matrix ** int.
     */
    public function testPowMatrixToInt(): void
    {
        $m = Matrix::fromArray([
            [1, 1],
            [0, 1],
        ]);
        $result = $m ** 2;

        $this->assertSame([
            [1.0, 2.0],
            [0.0, 1.0],
        ], $result->toArray());
        $this->assertEquals($m->pow(2), $result);
    }

    /**
     * Test Matrix ** negative int: equivalent to inverting first.
     */
    public function testPowMatrixToNegativeInt(): void
    {
        $m = Matrix::fromArray([
            [1, 2],
            [3, 4],
        ]);
        $result = $m ** -1;

        $this->assertEquals($m->inv(), $result);
    }

    /**
     * Test int ** Matrix is unsupported: a scalar raised to a matrix power isn't a standard
     * operation in linear algebra.
     */
    public function testPowIntToMatrixThrows(): void
    {
        $m = Matrix::fromArray([
            [1, 2],
            [3, 4],
        ]);

        $this->expectException(TypeError::class);
        $result = 2 ** $m; // @phpstan-ignore binaryOp.invalid
    }

    /**
     * Test Matrix ** Matrix is unsupported.
     */
    public function testPowMatrixToMatrixThrows(): void
    {
        $m1 = Matrix::fromArray([
            [1, 2],
            [3, 4],
        ]);
        $m2 = Matrix::fromArray([
            [1, 0],
            [0, 1],
        ]);

        $this->expectException(TypeError::class);
        $result = $m1 ** $m2;
    }

    /**
     * Test ** does not modify the operand.
     */
    public function testPowDoesNotMutate(): void
    {
        $m = Matrix::fromArray([
            [1, 1],
            [0, 1],
        ]);

        $result = $m ** 2;

        $this->assertSame([
            [1.0, 1.0],
            [0.0, 1.0],
        ], $m->toArray());
    }

    #endregion

    #region Inverting with A ** -1 tests.

    /**
     * Test that $m ** -1 and $m->inv() give the same result.
     */
    public function testInvertingEquivalence(): void
    {
        $m = Matrix::fromArray([
            [1, 2],
            [3, 4],
        ]);

        $inv = $m->inv();
        $viaPower = $m ** -1;

        $this->assertEquals($inv, $viaPower);
    }

    /**
     * Test that inverting via exponentiation throws ArithmeticException for a zero-determinant
     * Matrix, matching inv().
     */
    public function testInvertingSingularByPowThrows(): void
    {
        $singular = Matrix::fromArray([
            [1, 2],
            [2, 4],
        ]);

        $this->expectException(ArithmeticException::class);
        $result = $singular ** -1;
    }

    /**
     * Test that inverting via exponentiation throws DomainException for a non-square Matrix,
     * matching inv().
     */
    public function testInvertingNonSquareByPowThrows(): void
    {
        $nonSquare = Matrix::fromArray([
            [1, 2, 3],
            [4, 5, 6],
        ]);

        $this->expectException(DomainException::class);
        $result = $nonSquare ** -1;
    }

    #endregion
}
