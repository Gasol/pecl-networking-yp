--TEST--
yp_err_string() function test
--SKIPIF--
<?php if (!extension_loaded('yp')) die('Skip yp extension not loaded'); ?>
--FILE--
<?php
foreach (range(0, 16) as $errno) {
    var_dump(yp_err_string($errno));
}
?>
--EXPECT--
string(7) "Success"
string(21) "Request arguments bad"
string(28) "RPC failure on NIS operation"
string(45) "Can't bind to server which serves this domain"
string(30) "No such map in server's domain"
string(18) "No such key in map"
string(18) "Internal NIS error"
string(33) "Local resource allocation failure"
string(31) "No more records in map database"
string(33) "Can't communicate with portmapper"
string(29) "Can't communicate with ypbind"
string(29) "Can't communicate with ypserv"
string(25) "Local domain name not set"
string(23) "NIS map database is bad"
string(57) "NIS client/server version mismatch - can't supply service"
string(17) "Permission denied"
string(16) "Database is busy"
