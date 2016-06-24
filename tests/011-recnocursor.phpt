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
var_dump($c = new UpscaledbCursor($db));
var_dump($c->insert("key", "record"));

$k = 0;
var_dump($c->insert($k, "record1"));
var_dump($k);
$k = 0;
var_dump($c->insert($k, "record2"));
var_dump($k);
$k = 0;
var_dump($c->insert($k, "record3"));
var_dump($k);
$k = 0;
var_dump($c->insert($k, "record4"));
var_dump($k);
$k = 0;
var_dump($c->insert($k, "record5"));
var_dump($k);
$k = 0;
var_dump($c->find(1));
var_dump($c->erase());
dump($db);
var_dump($c->close());


var_dump($db = $env->create_db(2, UPS_RECORD_NUMBER64));
var_dump($c = new UpscaledbCursor($db));
var_dump($c->insert("key", "record"));

$k = 0;
var_dump($c->insert($k, "record1"));
var_dump($k);
$k = 0;
var_dump($c->insert($k, "record2"));
var_dump($k);
$k = 0;
var_dump($c->insert($k, "record3"));
var_dump($k);
$k = 0;
var_dump($c->insert($k, "record4"));
var_dump($k);
$k = 0;
var_dump($c->insert($k, "record5"));
var_dump($k);
$k = 0;
var_dump($c->find(1));
var_dump($c->erase());
dump($db);
var_dump($c->close());
--EXPECTF--
int(0)
object(UpscaledbDatabase)#2 (0) {
}
object(UpscaledbCursor)#3 (0) {
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
int(0)
object(UpscaledbDatabase)#4 (0) {
}
object(UpscaledbCursor)#2 (0) {
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
int(0)
