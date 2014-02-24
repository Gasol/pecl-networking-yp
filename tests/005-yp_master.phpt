--TEST--
yp_master() function test
--SKIPIF--
<?php if (!extension_loaded('yp')) die('Skip yp extension not loaded'); ?>
--FILE--
<?php
$domain = yp_get_default_domain();
var_dump(yp_master($domain, 'hosts.byaddr'));
?>
--EXPECTF--
string(%d) "%s"

