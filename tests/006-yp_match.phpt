--TEST--
yp_match() function test
--SKIPIF--
<?php if (!extension_loaded('yp')) die('Skip yp extension not loaded'); ?>
--FILE--
<?php
$domain = yp_get_default_domain();
var_dump(yp_match($domain, 'hosts.byaddr', '127.0.0.1'));
var_dump(yp_match($domain, 'hosts.byaddr', '0.0.0.0')); // not exists
?>
--EXPECTF--
string(19) "127.0.0.1	localhost"

Warning: yp_match(): No such key in map in %s/tests/006-yp_match.php on line 4
bool(false)
