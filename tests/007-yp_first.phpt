--TEST--
yp_first() function test
--SKIPIF--
<?php if (!extension_loaded('yp')) die('Skip yp extension not loaded'); ?>
--FILE--
<?php
$domain = yp_get_default_domain();
var_dump(yp_first($domain, 'group.byname'));
var_dump(yp_first($domain, 'passwd.byname'));
?>
--EXPECT--
array(1) {
  ["vagrant"]=>
  string(15) "vagrant:x:1000:"
}
array(1) {
  ["vagrant"]=>
  string(54) "vagrant:x:1000:1000:vagrant,,,:/home/vagrant:/bin/bash"
}
