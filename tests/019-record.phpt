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
$obj = (object)[
    'ip'=>'192.168.0.1',
    'bytes_in'=>'246001',
    'bytes_out'=>'475002',
    'username'=>'Omid',
    'hostname'=>'google',
    'browser'=>'firefox 58',
    'url'=>'http://google.com',
    'domain'=>'google',
    'code'=>'400',
    'sourceAddress'=>'192.168.0.14',
    'destinationAddress'=>'14.150.33.12'
];
$str = serialize($obj);
var_dump($env->create($path));
var_dump($db = $env->create_db(1));
var_dump($db->insert(null, $str, $str));
var_dump($db->find(null, $str));
var_dump($db->erase(null, $str));
var_dump($db->find(null, $str));
?>
--EXPECTF--
int(0)
object(UpscaledbDatabase)#3 (0) {
}
int(0)
string(360) "O:8:"stdClass":11:{s:2:"ip";s:11:"192.168.0.1";s:8:"bytes_in";s:6:"246001";s:9:"bytes_out";s:6:"475002";s:8:"username";s:4:"Omid";s:8:"hostname";s:6:"google";s:7:"browser";s:10:"firefox 58";s:3:"url";s:17:"http://google.com";s:6:"domain";s:6:"google";s:4:"code";s:3:"400";s:13:"sourceAddress";s:12:"192.168.0.14";s:18:"destinationAddress";s:12:"14.150.33.12";}"
int(0)
int(-11)
