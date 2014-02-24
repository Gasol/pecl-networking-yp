--TEST--
yp_first() and yp_order() function test
--SKIPIF--
<?php if (!extension_loaded('yp')) die('Skip yp extension not loaded'); ?>
--FILE--
<?php
$domain = yp_get_default_domain();
$mapname = 'group.byname';

var_dump(YPERR_NOMORE !== yp_errno());

$first = yp_first($domain, $mapname);
$groups[] = current($first);
$name = key($first);
while ($next = yp_next($domain, $mapname, $name)) {
    $name = key($next);
    $group = current($next);
    $groups[] = $group;
}
var_dump($next);
var_dump(YPERR_NOMORE === yp_errno());
sort($groups);
var_dump($groups);
?>
--EXPECTF--
bool(true)

Warning: yp_next(): No more records in map database in %s/tests/007-yp_first-and-yp_next.php on line 10
bool(false)
bool(true)
array(3) {
  [0]=>
  string(14) "puppet:x:1001:"
  [1]=>
  string(15) "vagrant:x:1000:"
  [2]=>
  string(14) "vboxsf:x:1002:"
}
