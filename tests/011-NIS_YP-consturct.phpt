--TEST--
NIS\YP::__construct() test
--SKIPIF--
<?php if (!extension_loaded('yp')) die('Skip yp extension not loaded'); ?>
--FILE--
<?php
$yp = new NIS\YP();
var_dump($yp->getDomain());
$yp = new NIS\YP('foo');
var_dump($yp->getDomain());
var_dump($yp->setDomain('bar'));
var_dump($yp->getDomain());
?>
--EXPECT--
string(9) "precise32"
string(3) "foo"
NULL
string(3) "bar"

