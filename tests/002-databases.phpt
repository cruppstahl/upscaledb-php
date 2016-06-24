--TEST--
upscaledb - create, close and open databases
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include "upscaledb.inc";
cleanup_upscaledb_on_shutdown();

$path = dirname(__FILE__) . '/upscaledb-basic.test-db';
$env = new Upscaledb();
var_dump($env->create($path));
var_dump($env->get_database_names());

var_dump($db1 = $env->create_db(1));
var_dump($env->get_database_names());

var_dump($db2 = $env->create_db(2));
var_dump($env->get_database_names());
var_dump($db3 = $env->create_db(3));
var_dump($env->get_database_names());
var_dump($env->create_db(1));
var_dump($db1->close());
var_dump($db2->close());
var_dump($db3->close());
var_dump($db1 = $env->open_db(1));
var_dump($env->get_database_names());
var_dump($db2 = $env->open_db(2));
var_dump($db3 = $env->open_db(3));
var_dump($env->open_db(4));

var_dump($env->close());
var_dump($db1->close());
var_dump($db2->close());
var_dump($db3->close());
var_dump($env->close());

var_dump($env->open($path));

var_dump($env->erase_db(1));
var_dump($env->get_database_names());
var_dump($env->rename_db(2, 5));
var_dump($env->get_database_names());

?>
--EXPECTF--
int(0)
array(0) {
}
object(UpscaledbDatabase)#2 (0) {
}
array(1) {
  [0]=>
  int(1)
}
object(UpscaledbDatabase)#3 (0) {
}
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
object(UpscaledbDatabase)#4 (0) {
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
int(-201)
int(0)
int(0)
int(0)
object(UpscaledbDatabase)#5 (0) {
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
object(UpscaledbDatabase)#2 (0) {
}
object(UpscaledbDatabase)#3 (0) {
}
int(-200)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
int(0)
array(2) {
  [0]=>
  int(5)
  [1]=>
  int(3)
}
