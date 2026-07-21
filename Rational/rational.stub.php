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

final class Rational
{
    public int $numerator = 0;

    public int $denominator = 1;

    public function __construct(int $num = 0, int $den = 1) {}
}
