<?php

declare(strict_types=1);

namespace OceanMoon\Math\Tests\Vector;

use Error;
use OceanMoon\Math\Vector;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\TestCase;
use ReflectionProperty;

/**
 * Tests that a Vector whose $data or $count was changed through Reflection throws Error, instead of crashing. Only the
 * extension checks this: the package throws whatever PHP throws by itself.
 */
#[CoversClass(Vector::class)]
class VectorInvalidDataTest extends TestCase
{
    #region Invalid $data tests.

    /**
     * Ways to break the invariant that $data holds $count floats.
     *
     * @return array<string, array{string, mixed}>
     */
    public static function tamperProvider(): array
    {
        return [
            'empty data'       => [
                'data',
                [],
            ],
            'string elements'  => [
                'data',
                ['x', 'y'],
            ],
            'integer elements' => [
                'data',
                [1, 2],
            ],
            'count too large'  => ['count', 5],
        ];
    }

    /**
     * Test reading an element of a tampered Vector throws Error.
     */
    #[DataProvider('tamperProvider')]
    public function testReadTamperedThrows(string $property, mixed $value): void
    {
        $v = Vector::fromArray([1.0, 2.0]);
        new ReflectionProperty($v, $property)->setValue($v, $value);

        $this->expectException(Error::class);
        $this->expectExceptionMessage('Vector data is invalid.');
        $v->get($v->count - 1);
    }

    /**
     * Test the other read paths of a tampered Vector throw Error too.
     */
    public function testOtherReadsOfTamperedThrow(): void
    {
        $v = Vector::fromArray([1.0, 2.0]);
        new ReflectionProperty($v, 'data')->setValue($v, []);
        $reads = [
            'offsetGet' => static fn () => $v[1],
            'magnitude' => static fn () => $v->magnitude,
            'add'       => static fn () => $v->add(Vector::fromArray([1.0, 2.0])),
            'toString'  => static fn () => (string) $v,
        ];

        foreach ($reads as $name => $read) {
            try {
                $read();
                $this->fail("Expected Error from $name.");
            } catch (Error $e) {
                $this->assertSame('Vector data is invalid.', $e->getMessage(), $name);
            }
        }
    }

    /**
     * Test the methods that read every element at once throw Error for a tampered Vector, including one whose count is
     * too large to allocate or loop over.
     */
    #[DataProvider('tamperProvider')]
    #[DataProvider('hugeCountProvider')]
    public function testBulkReadsOfTamperedThrow(string $property, mixed $value): void
    {
        $v = Vector::fromArray([1.0, 2.0]);
        new ReflectionProperty($v, $property)->setValue($v, $value);
        $reads = [
            'neg'         => static fn () => $v->neg(),
            'hadamardDiv' => static fn () => $v->hadamardDiv($v),
            'mul'         => static fn () => $v->mul(2.0),
            '/'           => static fn () => 2.0 / $v,
            'sum'         => static fn () => $v->sum(),
            'prod'        => static fn () => $v->prod(),
            'dot'         => static fn () => $v->dot($v),
            'magnitude'   => static fn () => $v->magnitude,
            'normalize'   => static fn () => $v->normalize(),
            'equal'       => static fn () => $v->equal($v),
            'approxEqual' => static fn () => $v->approxEqual($v),
            'toString'    => static fn () => (string) $v,
            'toRowMatrix' => static fn () => $v->toRowMatrix(),
            'json_encode' => static fn () => json_encode($v),
        ];

        foreach ($reads as $name => $read) {
            try {
                $read();
                $this->fail("Expected Error from $name.");
            } catch (Error $e) {
                $this->assertSame('Vector data is invalid.', $e->getMessage(), $name);
            }
        }
    }

    /**
     * Counts too large to allocate a buffer for or loop over.
     *
     * @return array<string, array{string, int}>
     */
    public static function hugeCountProvider(): array
    {
        return [
            'huge count'     => ['count', PHP_INT_MAX],
            'negative count' => ['count', -1],
        ];
    }

    #endregion
}
