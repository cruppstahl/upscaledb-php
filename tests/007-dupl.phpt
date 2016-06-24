--TEST--
upscaledb - insert, erase, lookup w/ database functions
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include "upscaledb.inc";
cleanup_upscaledb_on_shutdown();

$path = dirname(__FILE__) . '/upscaledb-basic.test-db';
$env = new Upscaledb();
var_dump($env->create($path));
var_dump($db = $env->create_db(1, UPS_ENABLE_DUPLICATE_KEYS));
var_dump($db->insert(null, "key", "record1"));
var_dump($db->insert(null, "key", "record2", UPS_DUPLICATE));
var_dump($db->insert(null, "key", "record3", UPS_DUPLICATE));
var_dump($db->find(null, "key"));
var_dump($db->erase(null, "key"));
var_dump($db->find(null, "key"));
?>
--EXPECTF--
int(0)
object(UpscaledbDatabase)#2 (0) {
}
int(0)
int(0)
int(0)
string(7) "record1"
int(0)
int(-11)
