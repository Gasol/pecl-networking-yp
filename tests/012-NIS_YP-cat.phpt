--TEST--
NIS\YP::cat() test
--SKIPIF--
<?php if (!extension_loaded('yp')) die('Skip yp extension not loaded'); ?>
--FILE--
<?php
$yp = new NIS\YP();
var_dump($yp->cat('hosts.byname'));
?>
--EXPECT--
array(2) {
  ["localhost"]=>
  string(19) "127.0.0.1	localhost"
  ["precise32"]=>
  string(19) "127.0.1.1	precise32"
}
