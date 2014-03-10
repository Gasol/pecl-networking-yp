--TEST--
NIS\YP::all() test
--SKIPIF--
<?php if (!extension_loaded('yp')) die('Skip yp extension not loaded'); ?>
--FILE--
<?php
$yp = new NIS\YP();
var_dump($yp->all('passwd.byname', function($status, $key, $value) {
    print_r(func_get_args());
    return true;
}));
?>
--EXPECT--
Array
(
    [0] => 1
    [1] => vagrant
    [2] => vagrant:x:1000:1000:vagrant,,,:/home/vagrant:/bin/bash
)
NULL
