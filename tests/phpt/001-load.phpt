--TEST--
Extension loads and registers Mossy2100\Math\Complex
--EXTENSIONS--
mossy2100_math
--FILE--
<?php
var_dump(extension_loaded('mossy2100_math'));
var_dump(class_exists('Mossy2100\\Math\\Complex'));

$rc = new ReflectionClass('Mossy2100\\Math\\Complex');
var_dump($rc->isFinal());
var_dump($rc->implementsInterface('Stringable'));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
