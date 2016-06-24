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
$params = array(
    UPS_PARAM_KEY_TYPE  => UPS_TYPE_UINT32
);
var_dump($db = $env->create_db(1, 0, $params));

var_dump($db->insert(null, 1, "record1"));
var_dump($db->insert(null, 2, "record2"));
var_dump($db->insert(null, 3, "record3"));
var_dump($db->insert(null, 4, "record4"));

var_dump($db->find(null, 1));
var_dump($db->find(null, 2));
var_dump($db->find(null, 3));
var_dump($db->find(null, 4));
var_dump($db->find(null, 5));

var_dump($db->erase(null, 0));
var_dump($db->erase(null, 1));

var_dump($db->find(null, 1));
var_dump($db->find(null, 2));
var_dump($db->find(null, 3));
var_dump($db->find(null, 4));

echo("64bit keys------------------------------\n");
$params = array(
    UPS_PARAM_KEY_TYPE  => UPS_TYPE_UINT64
);
var_dump($db = $env->create_db(2, 0, $params));

var_dump($db->insert(null, 1, "record1"));
var_dump($db->insert(null, 2, "record2"));
var_dump($db->insert(null, 3, "record3"));
var_dump($db->insert(null, 4, "record4"));

var_dump($db->find(null, 1));
var_dump($db->find(null, 2));
var_dump($db->find(null, 3));
var_dump($db->find(null, 4));
var_dump($db->find(null, 5));

var_dump($db->erase(null, 0));
var_dump($db->erase(null, 1));

var_dump($db->find(null, 1));
var_dump($db->find(null, 2));
var_dump($db->find(null, 3));
var_dump($db->find(null, 4));
?>
--EXPECTF--
int(0)
object(UpscaledbDatabase)#2 (0) {
}
int(0)
int(0)
int(0)
int(0)
string(7) "record1"
string(7) "record2"
string(7) "record3"
string(7) "record4"
int(-11)
int(-11)
int(0)
int(-11)
string(7) "record2"
string(7) "record3"
string(7) "record4"
64bit keys------------------------------
object(UpscaledbDatabase)#3 (0) {
}
int(0)
int(0)
int(0)
int(0)
string(7) "record1"
string(7) "record2"
string(7) "record3"
string(7) "record4"
int(-11)
int(-11)
int(0)
int(-11)
string(7) "record2"
string(7) "record3"
string(7) "record4"
