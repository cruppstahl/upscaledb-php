#!/bin/sh

ARGS="-n -d extension_dir=modules/ -d extension=upscaledb.so"
PHP=/usr/bin/php

$PHP $ARGS -f "/home/ruppc/prj/upscaledb-php/samples/db1.php"  2>&1
$PHP $ARGS -f "/home/ruppc/prj/upscaledb-php/samples/uqi1.php"  2>&1
