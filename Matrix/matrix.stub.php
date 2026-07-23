<?php

/**
 * Signature stub for the matrix extension (source of truth for arginfo).
 *
 * This mirrors the public shape the extension exposes and is the source of
 * truth for matrix_arginfo.h. Regenerate that header after editing this file:
 *
 *   php $(php-config --prefix)/lib/php/build/gen_stub.php Matrix/matrix.stub.php
 *
 * @generate-class-entries
 */

namespace OceanMoon\Math;

final class Matrix implements \Stringable, \Countable, \ArrayAccess
{
    private array $data = [];

    public int $rowCount = 0;

    public int $columnCount = 0;

    public function __construct(int $rowCount, int $columnCount) {}

    public static function fromArray(array $arr): Matrix {}

    public static function identity(int $size): Matrix {}

    public function toArray(): array {}

    public function __toString(): string {}

    public function isSquare(?int $size = null): bool {}

    public function get(int $row, int $col): float {}

    public function getRow(int $row): Vector {}

    public function getColumn(int $col): Vector {}

    public function copy(int $row, int $col, int $rowCount, int $colCount): Matrix {}

    public function set(int $row, int $col, float $value): void {}

    public function setRow(int $row, Vector $vec): void {}

    public function setColumn(int $col, Vector $vec): void {}

    public function paste(Matrix $other, int $row = 0, int $col = 0): void {}

    public function equal(mixed $other): bool {}

    public function approxEqual(mixed $other, float $relTol = 1e-9, float $absTol = PHP_FLOAT_EPSILON): bool {}

    public function resize(int $rowCount, int $columnCount): Matrix {}

    public function neg(): Matrix {}

    public function reciprocal(): Matrix {}

    public function inv(): Matrix {}

    public function add(Matrix $other): Matrix {}

    public function sub(Matrix $other): Matrix {}

    public function mul(float|Matrix $other): Matrix {}

    public function div(float $scalar): Matrix {}

    public function hadamardMul(Matrix $other): Matrix {}

    public function hadamardDiv(Matrix $other): Matrix {}

    public function pow(int $exponent): Matrix {}

    public function sqr(): Matrix {}

    public function mulVector(Vector $vector): Vector {}

    public function t(): Matrix {}

    public function det(): float {}

    public function trace(): float {}

    public function norm(): float {}

    public function p1Norm(): float {}

    public function pInfNorm(): float {}

    public function count(): int {}

    public function offsetExists(mixed $offset): bool {}

    public function offsetGet(mixed $offset): Vector {}

    public function offsetSet(mixed $offset, mixed $value): void {}

    public function offsetUnset(mixed $offset): void {}
}
