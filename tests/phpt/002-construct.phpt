--TEST--
Construct with real/imaginary parts and stringify
--EXTENSIONS--
complex
--FILE--
<?php
use OceanMoon\Math\Complex;

$z = new Complex(3, 4);
var_dump($z->real);
var_dump($z->imaginary);
echo $z, "\n";

$r = new Complex(5);
echo $r, "\n";

$zero = new Complex();
echo $zero, "\n";
?>
--EXPECT--
float(3)
float(4)
3+4i
5
0
