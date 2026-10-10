<?php

declare(strict_types=1);

namespace OceanMoon\Math\Tests\Rational;

use OceanMoon\Math\Rational;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\TestCase;

/**
 * Tests for casting a Rational, only available in the extension - see docs/Reference/Rational.md in this repo.
 *
 * PHPStan has no extension point for casts, so it reports `(float) $r`, `(int) $r`, floatval($r) and intval($r) as
 * errors. See docs/Usage/Static_Analysis.md.
 */
#[CoversClass(Rational::class)]
class RationalCastTest extends TestCase
{
    #region Float cast tests.

    /**
     * Data for float casts.
     *
     * @return array<string, array{int, int}> Numerator and denominator.
     */
    public static function floatCastProvider(): array
    {
        return [
            'zero'          => [0, 1],
            'integer'       => [5, 1],
            'fraction'      => [3, 4],
            'negative'      => [-3, 4],
            'repeating'     => [1, 3],
            'large integer' => [PHP_INT_MAX, 1],
            'tiny'          => [1, PHP_INT_MAX],
        ];
    }

    /**
     * Test (float) gives the same value as toFloat().
     */
    #[DataProvider('floatCastProvider')]
    public function testFloatCast(int $num, int $den): void
    {
        $r = new Rational($num, $den);

        $this->assertSame($r->toFloat(), (float) $r); // @phpstan-ignore cast.double
    }

    /**
     * Test floatval() and settype() go through the same cast as (float).
     */
    public function testFloatvalAndSettype(): void
    {
        $r = new Rational(-3, 4);
        $this->assertSame(-0.75, floatval($r)); // @phpstan-ignore argument.type

        settype($r, 'float');
        $this->assertSame(-0.75, $r);
    }

    #endregion

    #region Int cast tests.

    /**
     * Data for int casts.
     *
     * @return array<string, array{int, int, int}> Numerator, denominator, and expected int.
     */
    public static function intCastProvider(): array
    {
        return [
            'zero'             => [0, 1, 0],
            'integer'          => [5, 1, 5],
            'positive proper'  => [3, 4, 0],
            'negative proper'  => [-3, 4, 0],
            'positive'         => [7, 3, 2],
            'negative'         => [-7, 3, -2],
            'half'             => [5, 2, 2],
            'negative half'    => [-5, 2, -2],
            'largest integer'  => [PHP_INT_MAX, 1, PHP_INT_MAX],
            'smallest integer' => [-PHP_INT_MAX, 1, -PHP_INT_MAX],
            'large fraction'   => [PHP_INT_MAX, 2, 4611686018427387903],
        ];
    }

    /**
     * Test (int) gives the same value as trunc(), truncating toward zero.
     */
    #[DataProvider('intCastProvider')]
    public function testIntCast(int $num, int $den, int $expected): void
    {
        $r = new Rational($num, $den);

        $this->assertSame($expected, (int) $r); // @phpstan-ignore cast.int
        $this->assertSame($r->trunc(), (int) $r); // @phpstan-ignore cast.int
    }

    /**
     * Test (int) truncates like (int) on the equivalent float.
     */
    public function testIntCastMatchesFloatIntCast(): void
    {
        $r = new Rational(-7, 3);

        $this->assertSame((int) (-7 / 3), (int) $r); // @phpstan-ignore cast.int
    }

    /**
     * Test intval() and settype() go through the same cast as (int).
     */
    public function testIntvalAndSettype(): void
    {
        $r = new Rational(-7, 3);
        $this->assertSame(-2, intval($r)); // @phpstan-ignore argument.type

        settype($r, 'int');
        $this->assertSame(-2, $r);
    }

    #endregion

    #region Other cast tests.

    /**
     * Test (string) still gives __toString().
     */
    public function testStringCast(): void
    {
        $this->assertSame('-3/4', (string) new Rational(-3, 4));
    }

    /**
     * Test (bool) is true for any Rational, including zero, as for any object.
     */
    public function testBoolCast(): void
    {
        $this->assertTrue((bool) new Rational(0)); // @phpstan-ignore method.alreadyNarrowedType
    }

    #endregion
}
