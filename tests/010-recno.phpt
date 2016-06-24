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
var_dump($db = $env->create_db(1, UPS_RECORD_NUMBER32));
var_dump($db->insert(null, "key", "record"));

$k = 0;
var_dump($db->insert(null, $k, "record1"));
var_dump($k);
$k = 0;
var_dump($db->insert(null, $k, "record2"));
var_dump($k);
$k = 0;
var_dump($db->insert(null, $k, "record3"));
var_dump($k);
$k = 0;
var_dump($db->insert(null, $k, "record4"));
var_dump($k);
$k = 0;
var_dump($db->insert(null, $k, "record5"));
var_dump($k);
$k = 0;
var_dump($db->find(null, 1));
var_dump($db->erase(null, 1));
dump($db);


var_dump($db = $env->create_db(2, UPS_RECORD_NUMBER64));
var_dump($db->insert(null, "key", "record"));

$k = 0;
var_dump($db->insert(null, $k, "record1"));
var_dump($k);
$k = 0;
var_dump($db->insert(null, $k, "record2"));
var_dump($k);
$k = 0;
var_dump($db->insert(null, $k, "record3"));
var_dump($k);
$k = 0;
var_dump($db->insert(null, $k, "record4"));
var_dump($k);
$k = 0;
var_dump($db->insert(null, $k, "record5"));
var_dump($k);
$k = 0;
var_dump($db->find(null, 1));
var_dump($db->erase(null, 1));
dump($db);
--EXPECTF--
int(0)
object(UpscaledbDatabase)#2 (0) {
}
int(-8)
int(0)
int(1)
int(0)
int(2)
int(0)
int(3)
int(0)
int(4)
int(0)
int(5)
string(7) "record1"
int(0)
2 -> record2
3 -> record3
4 -> record4
5 -> record5
object(UpscaledbDatabase)#3 (0) {
}
int(-8)
int(0)
int(1)
int(0)
int(2)
int(0)
int(3)
int(0)
int(4)
int(0)
int(5)
string(7) "record1"
int(0)
2 -> record2
3 -> record3
4 -> record4
5 -> record5
