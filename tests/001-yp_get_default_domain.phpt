--TEST--
yp_get_default_domain() function test
--SKIPIF--
<?php if (!extension_loaded('yp')) die('Skip yp extension not loaded'); ?>
--FILE--
<?php
var_dump(yp_get_default_domain());
?>
--EXPECT--
string(9) "precise32"
