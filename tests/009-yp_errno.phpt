--TEST--
yp_errno() function test
--SKIPIF--
<?php if (!extension_loaded('yp')) die('Skip yp extension not loaded'); ?>
--FILE--
<?php
$constants = get_defined_constants(true);
asort($constants['yp']);
print_r($constants['yp']);

$domain = yp_get_default_domain();
var_dump(yp_errno());
var_dump(yp_next($domain, 'passwd.byname', 'vagrant'));
var_dump(yp_errno());
?>
--EXPECTF--
Array
(
    [YPERR_BADARGS] => 1
    [YPERR_RPC] => 2
    [YPERR_DOMAIN] => 3
    [YPERR_MAP] => 4
    [YPERR_KEY] => 5
    [YPERR_YPERR] => 6
    [YPERR_RESRC] => 7
    [YPERR_NOMORE] => 8
    [YPERR_PMAP] => 9
    [YPERR_YPBIND] => 10
    [YPERR_YPSERV] => 11
    [YPERR_NODOM] => 12
    [YPERR_BADDB] => 13
    [YPERR_VERS] => 14
    [YPERR_ACCESS] => 15
    [YPERR_BUSY] => 16
)
int(0)

Warning: yp_next(): No more records in map database in %s/tests/009-yp_errno.php on line 8
bool(false)
int(8)

