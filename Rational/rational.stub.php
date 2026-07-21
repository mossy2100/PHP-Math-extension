<?php

/**
 * Signature stub for the rational extension (source of truth for arginfo).
 *
 * This mirrors the public shape the extension exposes and is the source of
 * truth for rational_arginfo.h. Regenerate that header after editing this file:
 *
 *   php $(php-config --prefix)/lib/php/build/gen_stub.php Rational/rational.stub.php
 *
 * @generate-class-entries
 */

namespace OceanMoon\Math;

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
