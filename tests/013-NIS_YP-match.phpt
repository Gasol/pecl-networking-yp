--TEST--
NIS\YP::match() test
--SKIPIF--
<?php if (!extension_loaded('yp')) die('Skip yp extension not loaded'); ?>
--FILE--
<?php
$yp = new NIS\YP();
var_dump($yp->match('hosts.byaddr', '127.0.0.1'));
try {
    $yp->match('hosts.byaddr', '0.0.0.0'); // not exists
} catch (NIS\YPException $e) {
    var_dump($e->getCode());
    var_dump($e->getMessage());
}
?>
--EXPECT--
string(19) "127.0.0.1	localhost"
int(5)
string(18) "No such key in map"
