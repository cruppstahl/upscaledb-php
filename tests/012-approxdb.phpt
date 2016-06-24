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
var_dump($db->insert(null, "aaa", "record-aaa"));
var_dump($db->insert(null, "bbb", "record-bbb"));
var_dump($db->insert(null, "ccc", "record-ccc"));
var_dump($db->insert(null, "ddd", "record-ddd"));
var_dump($db->insert(null, "eee", "record-eee"));
dump($db);
var_dump($db->find(null, "ccc"));
var_dump($db->find(null, "ccc", UPS_FIND_LT_MATCH));
var_dump($db->find(null, "ccc", UPS_FIND_GT_MATCH));
var_dump($db->find(null, "ccc", UPS_FIND_GEQ_MATCH));
var_dump($db->find(null, "000", UPS_FIND_GT_MATCH));
var_dump($db->find(null, "fff", UPS_FIND_LT_MATCH));
?>
--EXPECTF--
int(0)
object(UpscaledbDatabase)#2 (0) {
}
int(0)
int(0)
int(0)
int(0)
int(0)
aaa -> record-aaa
bbb -> record-bbb
ccc -> record-ccc
ddd -> record-ddd
eee -> record-eee
string(10) "record-ccc"
string(10) "record-bbb"
string(10) "record-ddd"
string(10) "record-ccc"
string(10) "record-aaa"
string(10) "record-eee"
