--TEST--
Extension loads and registers OceanMoon\Math\Complex
--EXTENSIONS--
complex
--FILE--
<?php
var_dump(extension_loaded('complex'));
var_dump(class_exists('OceanMoon\\Math\\Complex'));

$rc = new ReflectionClass('OceanMoon\\Math\\Complex');
var_dump($rc->isFinal());
var_dump($rc->implementsInterface('Stringable'));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
