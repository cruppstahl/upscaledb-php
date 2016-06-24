<?php
/*
 * Copyright (C) 2005-2016 Christoph Rupp (chris@crupp.de).
 * All Rights Reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * See the file COPYING for License information.
 */

/*
 * This sample demonstrates a few simple UQI queries.
 */

// Our database will store 32bit integer keys and 32bit records
$params = array(
  UPS_PARAM_KEY_TYPE => UPS_TYPE_UINT32,
  UPS_PARAM_RECORD_TYPE => UPS_TYPE_UINT32
);

// First create a new upscaledb Environment
$env = new Upscaledb();
$st = $env->create("test.db", 0, 0664);
if ($st != UPS_SUCCESS)
  die("ups_env_create failed: $st");

// And in this Environment we create a new Database as described above.
// Incremented keys are assigned automatically
$db = $env->create_db(1, UPS_RECORD_NUMBER32, $params);
if (is_integer($db) && $db != UPS_SUCCESS)
  die("ups_env_create_db failed: $db");

// Now insert a bunch of key/record pairs. Keys are incremented sequentially,
// records have a random value.
for ($x = 0; $x <= 10000; $x++) {
  $key = 0;
  $record = rand(50, 100000);
  $st = $db->insert(null, $key, $record, 0);
  if ($st)
    die("ups_db_insert failed: $st");
} 

// Now use UQI to fetch the maximum record value
$result = $env->select_range("MAX(\$record) FROM DATABASE 1");
$key = $result->get_key(0);
$record = $result->get_record(0);
echo("MAX(\$record): key $key, record $record\n");
$result->close();

// ... and the minimum
$result = $env->select_range("MIN(\$record) FROM DATABASE 1");
$key = $result->get_key(0);
$record = $result->get_record(0);
echo("MIN(\$record): key $key, record $record\n");
$result->close();

// ... and the average of all records
$result = $env->select_range("AVERAGE(\$record) FROM DATABASE 1");
$record = $result->get_record(0);
echo("AVG(\$record): $record\n");
$result->close();

// Done! Clean up
$st = $db->close();
if ($st)
  die("ups_db_close failed: $st");
$st = $env->close();
if ($st)
  die("ups_env_close failed: $st");

echo("success!\n");
?>
