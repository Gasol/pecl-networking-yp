--TEST--
yp_next() function test
--SKIPIF--
<?php if (!extension_loaded('yp')) die('Skip yp extension not loaded'); ?>
--FILE--
<?php
$domain = yp_get_default_domain();
var_dump(yp_next($domain, 'group.byname', 'vagrant'));
var_dump(yp_next($domain, 'passwd.byname', 'vagrant'));
?>
--EXPECTF--
array(1) {
  ["puppet"]=>
  string(14) "puppet:x:1001:"
}

Warning: yp_next(): No more records in map database in %s/tests/008-yp_next.php on line 4
bool(false)
