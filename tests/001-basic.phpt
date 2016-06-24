--TEST--
upscaledb - create, close and open environments
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include "upscaledb.inc";
cleanup_upscaledb_on_shutdown();

$path = dirname(__FILE__) . '/upscaledb-basic.test-db';
$env = new Upscaledb();

var_dump($env->create($path));
var_dump($env->close());
var_dump($env->open($path));
var_dump($env->close());
var_dump($env->open("foo"));
?>
--EXPECTF--
int(0)
int(0)
int(0)
int(0)
1os/os_posix.cc[%d]: opening file foo failed with status 2 (No such file or directory)
int(-21)
