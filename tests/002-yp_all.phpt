--TEST--
yp_all() function test
--SKIPIF--
<?php if (!extension_loaded('yp')) die('Skip yp extension not loaded'); ?>
--FILE--
<?php
$domain = yp_get_default_domain();
var_dump(yp_all($domain, 'passwd.byname', function($status, $key, $value) {
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
bool(false)
