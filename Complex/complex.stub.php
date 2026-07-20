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

final class Complex implements \Stringable, \ArrayAccess
{
    public float $real = 0.0;

    public float $imaginary = 0.0;

    public ?float $magnitude = null;

    public ?float $phase = null;

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
