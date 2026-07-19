<?php

declare(strict_types=1);

// Enable class autoloading (PHPUnit\Framework\TestCase, etc.) for the extension's own tests.
require_once __DIR__ . '/../vendor/autoload.php';

// Define constant for floating-point comparison tolerance, used by tests symlinked in from
// packages/Math's shared/ conformance suite.
const EPSILON = 1e-10;
