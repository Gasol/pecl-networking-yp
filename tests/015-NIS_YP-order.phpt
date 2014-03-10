--TEST--
NIS\YP::order() function test
--SKIPIF--
<?php if (!extension_loaded('yp')) die('Skip yp extension not loaded'); ?>
--FILE--
<?php
$yp = new NIS\YP();
var_dump($yp->order('hosts.byaddr'));
?>
--EXPECTF--
int(%d)
