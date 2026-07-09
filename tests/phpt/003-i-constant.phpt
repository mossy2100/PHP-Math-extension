--TEST--
OceanMoon\Math\I constant is a Complex(0, 1), registered per-request
--EXTENSIONS--
complex
--FILE--
<?php
use OceanMoon\Math\Complex;

var_dump(defined('OceanMoon\\Math\\I'));

$i = constant('OceanMoon\\Math\\I');
var_dump($i instanceof Complex);
var_dump($i->real);
var_dump($i->imaginary);
echo $i, "\n";
?>
--EXPECT--
bool(true)
bool(true)
float(0)
float(1)
0+1i
