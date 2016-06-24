<?php

function cleanup_upscaledb_on_shutdown() {
  register_shutdown_function('cleanup_upscaledb');
}

function cleanup_upscaledb() {
  global $upscaledb_path, $db;
  if (file_exists($upscaledb_path)) {
    if($db instanceof Upscaledb) {
      $db->close();
    }
    Upscaledb::destroy($upscaledb_path);
  }
}

function dump($db) {
  $key = "";
  $record = "";
  $c = new UpscaledbCursor($db);
  while ($c->move($key, $record, UPS_CURSOR_NEXT) == 0) {
    echo("$key -> $record\n");
  }
  #$c->close();
}
