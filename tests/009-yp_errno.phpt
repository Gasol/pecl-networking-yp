--TEST--
yp_errno() function test
--SKIPIF--
<?php if (!extension_loaded('yp')) die('Skip yp extension not loaded'); ?>
--FILE--
<?php
$domain = yp_get_default_domain();
var_dump(yp_errno());
var_dump(yp_next($domain, 'passwd.byname', 'vagrant'));
var_dump(yp_errno());
?>
--EXPECT--
int(0)

Warning: yp_next(): No more records in map database in /vagrant/tests/009-yp_errno.php on line 4
bool(false)
int(8)
