<?php

/**
 * Signature stub for the complex extension (source of truth for arginfo).
 *
 * This mirrors the public shape the extension exposes and is the source of
 * truth for complex_arginfo.h. Regenerate that header after editing this file:
 *
 *   php $(php-config --prefix)/lib/php/build/gen_stub.php Complex/complex.stub.php
 *
 * @generate-class-entries
 */

namespace OceanMoon\Math;

final class Complex implements \Stringable
{
    public float $real = 0.0;

    public float $imaginary = 0.0;

    public function __construct(float $real = 0, float $imag = 0) {}

    public function __toString(): string {}

    public function toArray(): array {}

    public function toObject(): \stdClass {}

    public function isReal(): bool {}

    public function equal(mixed $other): bool {}

    public function approxEqual(mixed $other, float $relTol = 1e-9, float $absTol = PHP_FLOAT_EPSILON): bool {}

    public static function fromArray(array $arr): Complex {}

    public static function fromObject(object $obj): Complex {}

    public static function toComplex(mixed $value): Complex {}
}
