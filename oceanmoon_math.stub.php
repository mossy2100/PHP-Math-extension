<?php

/**
 * Signature stub for the oceanmoon_math extension (source of truth for arginfo).
 *
 * This mirrors the public shape the extension exposes and is the source of
 * truth for oceanmoon_math_arginfo.h. Regenerate that header after editing this file:
 *
 *   php $(php-config --prefix)/lib/php/build/gen_stub.php oceanmoon_math.stub.php
 *
 * @generate-class-entries
 */

namespace OceanMoon\Math;

final class Complex implements \Stringable, \ArrayAccess
{
    public float $real = 0.0;

    public float $imaginary = 0.0;

    public float $magnitude = 0.0;

    public float $phase = 0.0;

    public function __construct(float $real = 0, float $imag = 0) {}

    public static function fromString(string $str): Complex {}

    public static function fromPolar(float $mag, float $phase): Complex {}

    public function __toString(): string {}

    public function isReal(): bool {}

    public function equal(mixed $other): bool {}

    public function approxEqual(mixed $other, float $relTol = 1e-9, float $absTol = PHP_FLOAT_EPSILON): bool {}

    public function neg(): Complex {}

    public function inv(): Complex {}

    public function conj(): Complex {}

    public function add(Complex|float $other): Complex {}

    public function sub(Complex|float $other): Complex {}

    public function mul(Complex|float $other): Complex {}

    public function div(Complex|float $other): Complex {}

    public function pow(Complex|float $other): Complex {}

    public function sqr(): Complex {}

    /**
     * @return list<Complex>
     */
    public function roots(int $degree): array {}

    public function sqrt(): Complex {}

    public function exp(): Complex {}

    public function ln(): Complex {}

    public function log(Complex|float $base): Complex {}

    public function sin(): Complex {}

    public function cos(): Complex {}

    public function tan(): Complex {}

    public function asin(): Complex {}

    public function acos(): Complex {}

    public function atan(): Complex {}

    public function sinh(): Complex {}

    public function cosh(): Complex {}

    public function tanh(): Complex {}

    public function asinh(): Complex {}

    public function acosh(): Complex {}

    public function atanh(): Complex {}

    public function round(int $precision = 0, \RoundingMode $mode = \RoundingMode::HalfAwayFromZero): Complex {}

    public function offsetExists(mixed $offset): bool {}

    public function offsetGet(mixed $offset): float {}

    public function offsetSet(mixed $offset, mixed $value): void {}

    public function offsetUnset(mixed $offset): void {}
}

final class Rational implements \Stringable
{
    public int $numerator = 0;

    public int $denominator = 1;

    public function __construct(int $num = 0, int $den = 1) {}

    public static function fromFloat(float $value): Rational {}

    public static function fromString(string $str): Rational {}

    public function toFloat(): float {}

    /**
     * @return array{int, Rational}
     */
    public function toMixedNumber(): array {}

    public function __toString(): string {}

    public function compare(mixed $other): int {}

    public function equal(mixed $other): bool {}

    public function lessThan(mixed $other): bool {}

    public function lessThanOrEqual(mixed $other): bool {}

    public function greaterThan(mixed $other): bool {}

    public function greaterThanOrEqual(mixed $other): bool {}

    public function approxEqual(mixed $other, float $relTol = 1e-9, float $absTol = PHP_FLOAT_EPSILON): bool {}

    public function approxCompare(mixed $other, float $relTol = 1e-9, float $absTol = PHP_FLOAT_EPSILON): int {}

    public function abs(): Rational {}

    public function neg(): Rational {}

    public function inv(): Rational {}

    public function add(Rational|int $other): Rational {}

    public function sub(Rational|int $other): Rational {}

    public function mul(Rational|int $other): Rational {}

    public function div(Rational|int $other): Rational {}

    public function pow(int $exponent): Rational {}

    public function sqr(): Rational {}

    public function round(\RoundingMode $mode = \RoundingMode::HalfAwayFromZero): int {}

    public function floor(): int {}

    public function ceil(): int {}
}

final class Vector implements \Stringable, \Countable, \ArrayAccess
{
    public int $count = 0;

    private array $data = [];

    public readonly float $magnitude;

    public function __construct(int $count) {}

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

final class Matrix implements \Stringable, \Countable, \ArrayAccess
{
    public int $rowCount = 0;

    public int $columnCount = 0;

    private array $data = [];

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
