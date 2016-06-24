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
var_dump($db = $env->create_db(1));
var_dump($db->insert(null, "key", "record"));
var_dump($db->find(null, "key"));
var_dump($db->insert(null, "key", "record"));
var_dump($db->find(null, "key"));
var_dump($db->insert(null, "key", "record2", UPS_OVERWRITE));
var_dump($db->find(null, "key"));
var_dump($db->insert(null, "key1", "record", UPS_OVERWRITE));
var_dump($db->find(null, "key1"));
var_dump($db->find(null, "key2"));

var_dump($db->erase(null, "key2"));
var_dump($db->erase(null, "key1"));
var_dump($db->find(null, "key1"));
?>
--EXPECTF--
int(0)
object(UpscaledbDatabase)#2 (0) {
}
int(0)
string(6) "record"
int(-12)
string(6) "record"
int(0)
string(7) "record2"
int(0)
string(6) "record"
int(-11)
int(-11)
int(0)
int(-11)
