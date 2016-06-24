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

echo("about to unset(env)\n");
unset($env);
echo("about to unset(db)\n");
unset($db);
echo("about to unset(c)\n");
unset($c);
?>
--EXPECTF--
int(0)
object(UpscaledbDatabase)#2 (0) {
}
object(UpscaledbCursor)#3 (0) {
}
about to unset(env)
about to unset(db)
about to unset(c)
