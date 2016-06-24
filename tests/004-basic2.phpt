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

var_dump($env->create(null, UPS_IN_MEMORY));
var_dump($env->close());
var_dump($env->open(null, UPS_IN_MEMORY));

var_dump($env->create($path, UPS_ENABLE_FSYNC, 0644));
var_dump($env->close());

$params = array(
    UPS_PARAM_KEY_TYPE  => UPS_TYPE_UINT32,
    UPS_PARAM_RECORD_SIZE => 12
);

var_dump($env->create($path, UPS_ENABLE_FSYNC, 0644, $params));
var_dump($env->close());

$params = array(
    UPS_PARAM_PAGE_SIZE  => 1024 * 8,
    UPS_PARAM_CACHE_SIZE  => 100000
);

var_dump($env->create($path, UPS_ENABLE_FSYNC, 0644, $params));
var_dump($env->close());

?>
--EXPECTF--
int(0)
int(0)
5upscaledb/upscaledb.cc[%d]: cannot open an in-memory database
int(-8)
int(0)
int(0)
5upscaledb/upscaledb.cc[%d]: unknown parameter 260
int(-8)
int(0)
int(0)
int(0)
