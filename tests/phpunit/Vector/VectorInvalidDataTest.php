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

    #endregion
}
