<?php

/**
 * PHPStan-only declaration of OceanMoon\Math\M_I, the Complex(0, 1) constant.
 *
 * Not a real stub -- never processed by gen_stub.php (no @generate-class-entries here), and never
 * executed at runtime (only listed in phpstan.neon's scanFiles). The real M_I is registered purely
 * at request-start via zend_register_constant() in complex_rinit() (Complex/complex.c), fresh on
 * every request, so there's no PHP source text anywhere for PHPStan to discover it in otherwise --
 * gen_stub.php also can't generate this itself, since it requires constant expressions to be a
 * "global, non-magic constant" (object instantiation isn't allowed). This file exists solely to
 * give PHPStan something to see.
 */

namespace OceanMoon\Math;

const M_I = new Complex(0, 1);
