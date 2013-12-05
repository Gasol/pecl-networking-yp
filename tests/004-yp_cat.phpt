--TEST--
yp_cat() function test
--SKIPIF--
<?php if (!extension_loaded('yp')) die('Skip yp extension not loaded'); ?>
--FILE--
<?php
$domain = yp_get_default_domain();
var_dump(yp_cat($domain, 'hosts.byname'));
?>
--EXPECT--
array(2) {
  ["localhost"]=>
  string(19) "127.0.0.1	localhost"
  ["precise32"]=>
  string(19) "127.0.1.1	precise32"
}
