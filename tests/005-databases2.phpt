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

$params = array(
    UPS_PARAM_KEY_TYPE  => UPS_TYPE_UINT32,
    UPS_PARAM_RECORD_SIZE  => 12
);

var_dump($db1 = $env->create_db(1, 0, $params));
var_dump($env->get_database_names());

var_dump($db2 = $env->create_db(2, UPS_RECORD_NUMBER32));
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
