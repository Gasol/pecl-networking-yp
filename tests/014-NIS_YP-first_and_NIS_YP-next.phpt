--TEST--
NIS\YP::first() and NIS\YP::next() test
--SKIPIF--
<?php if (!extension_loaded('yp')) die('Skip yp extension not loaded'); ?>
--FILE--
<?php
$mapname = 'group.byname';

$yp = new NIS\YP();

$first = $yp->first($mapname);
$groups[] = current($first);
$name = key($first);
try {
    while ($next = $yp->next($mapname, $name)) {
        $name = key($next);
        $group = current($next);
        $groups[] = $group;
    }
} catch (NIS\YPException $e) {
    var_dump($e->getCode());
    var_dump($e->getMessage());
}
sort($groups);
var_dump($groups);
?>
--EXPECTF--
int(8)
string(31) "No more records in map database"
array(3) {
  [0]=>
  string(14) "puppet:x:1001:"
  [1]=>
  string(15) "vagrant:x:1000:"
  [2]=>
  string(14) "vboxsf:x:1002:"
}
