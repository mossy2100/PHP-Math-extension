<?php

declare(strict_types=1);

namespace OceanMoon\Math\Tests\Matrix;

use Error;
use OceanMoon\Math\Matrix;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\TestCase;
use ReflectionProperty;
use stdClass;

/**
 * Tests that a Matrix whose $data, $rowCount or $columnCount was changed through Reflection throws Error, instead of
 * crashing. Only the extension checks this: the package throws whatever PHP throws by itself.
 */
#[CoversClass(Matrix::class)]
class MatrixInvalidDataTest extends TestCase
{
    #region Invalid $data tests.

    /**
     * Ways to break the invariant that $data holds $rowCount Vectors of $columnCount elements.
     *
     * @return array<string, array{string, mixed}>
     */
    public static function tamperProvider(): array
    {
        return [
            'empty data'             => [
                'data',
                [],
            ],
            'non-Vector rows'        => [
                'data',
                [
                    new stdClass(),
                    'x',
                ],
            ],
            'column count too large' => ['columnCount', 3],
            'row count too large'    => ['rowCount', 3],
        ];
    }

    /**
     * Test reading an element of a tampered Matrix throws Error.
     */
    #[DataProvider('tamperProvider')]
    public function testReadTamperedThrows(string $property, mixed $value): void
    {
        $m = Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
        ]);
        new ReflectionProperty($m, $property)->setValue($m, $value);

        $this->expectException(Error::class);
        $this->expectExceptionMessage('Matrix data is invalid.');
        $m->get($m->rowCount - 1, $m->columnCount - 1);
    }

    /**
     * Test the other read and write paths of a tampered Matrix throw Error too.
     */
    public function testOtherAccessesOfTamperedThrow(): void
    {
        $m = Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
        ]);
        new ReflectionProperty($m, 'data')->setValue($m, []);
        $accesses = [
            'offsetGet'   => static fn () => $m[1],
            'getRow'      => static fn () => $m->getRow(1),
            'set'         => static fn () => $m->set(1, 1, 5.0),
            'det'         => static fn () => $m->det(),
            'mul'         => static fn () => $m->mul($m),
            'clone + get' => static fn () => (clone $m)->get(1, 1),
        ];

        foreach ($accesses as $name => $access) {
            try {
                $access();
                $this->fail("Expected Error from $name.");
            } catch (Error $e) {
                $this->assertSame('Matrix data is invalid.', $e->getMessage(), $name);
            }
        }
    }

    #endregion
}
