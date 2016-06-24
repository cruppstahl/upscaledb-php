--TEST--
upscaledb - create, close and open environments
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include "upscaledb.inc";
cleanup_upscaledb_on_shutdown();

var_dump(Upscaledb::strerror(0));
var_dump(Upscaledb::strerror(-1));
var_dump(Upscaledb::strerror(-2));
var_dump(Upscaledb::strerror(-3));
var_dump(Upscaledb::strerror(-4));
var_dump(Upscaledb::strerror(1));
?>
--EXPECTF--
string(7) "Success"
string(13) "Unknown error"
string(19) "Invalid record size"
string(16) "Invalid key size"
string(17) "Invalid page size"
string(13) "Unknown error"
