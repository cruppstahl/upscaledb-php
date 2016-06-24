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
var_dump($c = new UpscaledbCursor($db));

var_dump($c->insert("key1", "record1"));
var_dump($c->insert("key2", "record2"));
var_dump($c->insert("key3", "record3"));

dump($db);

var_dump($c->find("key1"));
var_dump($c->erase());

dump($db);

?>
--EXPECTF--
int(0)
object(UpscaledbDatabase)#2 (0) {
}
object(UpscaledbCursor)#3 (0) {
}
int(0)
int(0)
int(0)
key1 -> record1
key2 -> record2
key3 -> record3
string(7) "record1"
int(0)
key2 -> record2
key3 -> record3
