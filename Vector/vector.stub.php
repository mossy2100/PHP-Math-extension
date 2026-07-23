<?php

/**
 * Signature stub for the vector extension (source of truth for arginfo).
 *
 * This mirrors the public shape the extension exposes and is the source of
 * truth for vector_arginfo.h. Regenerate that header after editing this file:
 *
 *   php $(php-config --prefix)/lib/php/build/gen_stub.php Vector/vector.stub.php
 *
 * @generate-class-entries
 */

namespace OceanMoon\Math;

final class Vector implements \Stringable, \Countable, \ArrayAccess
{
    private array $data = [];

    public int $size = 0;

    public ?float $magnitude = null;

    public function __construct(int $size) {}

    public static function fromArray(array $arr): Vector {}

    public function toArray(): array {}

    public function toRowMatrix(): Matrix {}

    public function toColumnMatrix(): Matrix {}

    public function __toString(): string {}

    public function get(int $index): float {}

    public function set(int $index, float $value): void {}

    public function normalize(): void {}

    public function equal(mixed $other): bool {}

    public function approxEqual(mixed $other, float $relTol = 1e-9, float $absTol = PHP_FLOAT_EPSILON): bool {}

    public function neg(): Vector {}

    public function reciprocal(): Vector {}

    public function add(Vector $other): Vector {}

    public function sub(Vector $other): Vector {}

    public function mul(float|Matrix $other): Vector {}

    public function div(float $scalar): Vector {}

    public function hadamardMul(Vector $other): Vector {}

    public function hadamardDiv(Vector $other): Vector {}

    public function dot(Vector $other): float {}

    public function cross(Vector $other): Vector {}

    public function outer(Vector $other): Matrix {}

    public function normalized(): Vector {}

    public function sum(): float {}

    public function prod(): float {}

    public function count(): int {}

    public function offsetExists(mixed $offset): bool {}

    public function offsetGet(mixed $offset): float {}

    public function offsetSet(mixed $offset, mixed $value): void {}

    public function offsetUnset(mixed $offset): void {}
}
