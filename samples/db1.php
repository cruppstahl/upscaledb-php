<?php
/*
 * Copyright (C) 2005-2017 Christoph Rupp (chris@crupp.de).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
