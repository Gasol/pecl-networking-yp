--TEST--
yp_order() function test
--SKIPIF--
<?php if (!extension_loaded('yp')) die('Skip yp extension not loaded'); ?>
--FILE--
<?php
$domain = yp_get_default_domain();
var_dump(yp_order($domain, 'hosts.byaddr'));
?>
--EXPECTF--
int(%d)
