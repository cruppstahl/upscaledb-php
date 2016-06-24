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
 * A simple example which creates a database, inserts some values,
 * looks them up and erases them.
 */

// Our database will store 32bit integer keys, and 64bit records
$params = array(
  UPS_PARAM_KEY_TYPE => UPS_TYPE_UINT32,
  UPS_PARAM_RECORD_TYPE => UPS_TYPE_UINT64
);

// First create a new upscaledb Environment
$env = new Upscaledb();
$st = $env->create("test.db", 0, 0664);
if ($st != UPS_SUCCESS)
  die("ups_env_create failed: $st");

// And in this Environment we create a new Database for uint32-keys
$db = $env->create_db(1, 0, $params);
if (is_integer($db) && $db != UPS_SUCCESS)
  die("ups_env_create_db failed: $db");

// Now we can insert, delete or lookup values in the database
for ($x = 0; $x <= 10; $x++) {
  $st = $db->insert(null, $x, $x, 0);
  if ($st)
    die("ups_db_insert failed: $st");
} 

for ($x = 0; $x <= 10; $x++) {
  $y = (int)$db->find(null, $x);
  if ($x != $y)
    die("ups_db_find: unexpected value $x != $y");
} 

// Done! Clean up
$st = $db->close();
if ($st)
  die("ups_db_close failed: $st");
$st = $env->close();
if ($st)
  die("ups_env_close failed: $st");

echo("success!\n");
?>
