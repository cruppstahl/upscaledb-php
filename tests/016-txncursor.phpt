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
var_dump($env->create($path, UPS_ENABLE_TRANSACTIONS));
var_dump($db = $env->create_db(1));

var_dump($txn = new UpscaledbTransaction($env));
var_dump($c = new UpscaledbCursor($db, $txn));
var_dump($c->insert("foo", "record"));
var_dump($db->find(null, "foo"));
var_dump($c->find("foo"));
var_dump($c->find("foo"));
var_dump($c->close());
var_dump($txn->commit());

var_dump($txn = new UpscaledbTransaction($env));
var_dump($c = new UpscaledbCursor($db, $txn));
var_dump($c->insert("bar", "record"));
var_dump($db->find(null, "bar"));
var_dump($c->find("bar"));
var_dump($c->close());
var_dump($txn->abort());

var_dump($db->find(null, "foo"));
var_dump($db->find(null, "bar"));
?>
--EXPECTF--
int(0)
object(UpscaledbDatabase)#2 (0) {
}
object(UpscaledbTransaction)#3 (0) {
}
object(UpscaledbCursor)#4 (0) {
}
int(0)
int(-31)
string(6) "record"
string(6) "record"
int(0)
int(0)
object(UpscaledbTransaction)#5 (0) {
}
object(UpscaledbCursor)#3 (0) {
}
int(0)
int(-31)
string(6) "record"
int(0)
int(0)
int(-11)
int(-11)
