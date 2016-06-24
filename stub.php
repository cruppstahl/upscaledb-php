<?php

/**
 * upscaledb extension stub file for code completion purposes
 *
 * WARNING: Do not include this file
 *
 */

namespace Upscaledb;

// A binary blob without type; sorted by memcmp
define("UPS_TYPE_BINARY", 0);
// A binary blob without type; sorted by callback function
// define("UPS_TYPE_CUSTOM", 1); - not yet implemented in the PHP wrapper
// An unsigned 8-bit integer
define("UPS_TYPE_UINT8", 3);
// An unsigned 16-bit integer
define("UPS_TYPE_UINT16", 5);
// An unsigned 32-bit integer
define("UPS_TYPE_UINT32", 7);
// An unsigned 64-bit integer
define("UPS_TYPE_UINT64", 9);
// An 32-bit float
define("UPS_TYPE_REAL32", 11);
// An 64-bit double
define("UPS_TYPE_REAL64", 12);

// Operation was successful
define("UPS_SUCCESS", 0);
// Invalid record size
define("UPS_INV_RECORD_SIZE", -2);
// Invalid key size
define("UPS_INV_KEY_SIZE", -3);
// Invalid page size (must be 1024 or a multiple of 2048)
define("UPS_INV_PAGE_SIZE", -4);
// Memory allocation failed - out of memory
define("UPS_OUT_OF_MEMORY", -6);
// Invalid function parameter
define("UPS_INV_PARAMETER", -8);
// Invalid file header
define("UPS_INV_FILE_HEADER", -9);
// Invalid file version
define("UPS_INV_FILE_VERSION", -10);
// Key was not found
define("UPS_KEY_NOT_FOUND", -11);
// Tried to insert a key which already exists
define("UPS_DUPLICATE_KEY", -12);
// Internal Database integrity violated
define("UPS_INTEGRITY_VIOLATED", -13);
// Internal upscaledb error
define("UPS_INTERNAL_ERROR", -14);
// Tried to modify the Database, but the file was opened as read-only
define("UPS_WRITE_PROTECTED", -15);
// Database record not found
define("UPS_BLOB_NOT_FOUND", -16);
// Generic file I/O error
define("UPS_IO_ERROR", -18);
// Function is not yet implemented
define("UPS_NOT_IMPLEMENTED", -20);
// File not found
define("UPS_FILE_NOT_FOUND", -21);
// Operation would block
define("UPS_WOULD_BLOCK", -22);
// Object was not initialized correctly
define("UPS_NOT_READY", -23);
// Database limits reached
define("UPS_LIMITS_REACHED", -24);
// Object was already initialized
define("UPS_ALREADY_INITIALIZED", -27);
// Database needs recovery
define("UPS_NEED_RECOVERY", -28);
// Cursor must be closed prior to Txn abort/commit
define("UPS_CURSOR_STILL_OPEN", -29);
// Record filter or file filter not found
define("UPS_FILTER_NOT_FOUND", -30);
// Operation conflicts with another Txn
define("UPS_TXN_CONFLICT", -31);
// Database cannot be closed because it is modified in a Txn
define("UPS_TXN_STILL_OPEN", -33);
// Cursor does not point to a valid item
define("UPS_CURSOR_IS_NIL", -100);
// Database not found
define("UPS_DATABASE_NOT_FOUND", -200);
// Database name already exists
define("UPS_DATABASE_ALREADY_EXISTS", -201);
// Database already open, or: Database handle is already initialized
define("UPS_DATABASE_ALREADY_OPEN", -202);
// Environment already open, or: Environment handle is already initialized
define("UPS_ENVIRONMENT_ALREADY_OPEN", -203);
// Invalid log file header
define("UPS_LOG_INV_FILE_HEADER", -300);
// Remote I/O error/Network error
define("UPS_NETWORK_ERROR", -400);
// UQI: plugin not found or unable to load
define("UPS_PLUGIN_NOT_FOUND", -500);
// UQI: failed to parse a query command
define("UPS_PARSER_ERROR", -501);
// UQI: a plugin with the given name is already registered
define("UPS_PLUGIN_ALREADY_EXISTS", -502);

// Flag for ups_txn_begin
define("UPS_TXN_READ_ONLY", 1);

// Flag for ups_env_open, ups_env_create.
define("UPS_ENABLE_FSYNC", 0x00000001);
// Flag for ups_env_open, ups_env_open_db.
define("UPS_READ_ONLY", 0x00000004);
// Flag for ups_env_create.
define("UPS_IN_MEMORY", 0x00000080);
// Flag for ups_env_open, ups_env_create.
define("UPS_DISABLE_MMAP", 0x00000200);
// Flag for ups_env_create_db.
define("UPS_RECORD_NUMBER32", 0x00001000);
// Flag for ups_env_create_db.
define("UPS_RECORD_NUMBER64", 0x00002000);
// Flag for ups_env_create_db.
define("UPS_ENABLE_DUPLICATE_KEYS", 0x00004000);
// Flag for ups_env_open.
define("UPS_AUTO_RECOVERY", 0x00010000);
// Flag for ups_env_create, ups_env_open.
define("UPS_ENABLE_TRANSACTIONS", 0x00020000);
// Flag for ups_env_open, ups_env_create.
define("UPS_CACHE_UNLIMITED", 0x00040000);
// Flag for ups_env_create, ups_env_open.
define("UPS_DISABLE_RECOVERY", 0x00080000);
// Flag for ups_env_open, ups_env_create.
define("UPS_ENABLE_CRC32", 0x02000000);
// Flag for ups_env_open, ups_env_create.
define("UPS_FLUSH_TRANSACTIONS_IMMEDIATELY", 0x08000000);

// Flag for ups_db_insert and ups_cursor_insert
define("UPS_OVERWRITE", 0x0001);
// Flag for ups_db_insert and ups_cursor_insert
define("UPS_DUPLICATE", 0x0002);
// Flag for ups_cursor_insert
define("UPS_DUPLICATE_INSERT_BEFORE", 0x0004);
// Flag for ups_cursor_insert
define("UPS_DUPLICATE_INSERT_AFTER", 0x0008);
// FlagFlag for ups_cursor_insert
define("UPS_DUPLICATE_INSERT_FIRST", 0x0010);
// Flag for ups_cursor_insert
define("UPS_DUPLICATE_INSERT_LAST", 0x0020);

// Flag for ups_cursor_insert
define("UPS_HINT_APPEND", 0x00080000);
// Flag for ups_cursor_insert
define("UPS_HINT_PREPEND", 0x00100000);

// Parameter name for ups_env_open, ups_env_create;
define("UPS_PARAM_JOURNAL_SWITCH_THRESHOLD", 0x00001);
// Parameter name for ups_env_open, ups_env_create;
define("UPS_PARAM_CACHE_SIZE", 0x00000100);
// Parameter name for ups_env_create; sets the page size
define("UPS_PARAM_PAGE_SIZE", 0x00000101);
// Parameter name for ups_env_create_db; sets the key size
define("UPS_PARAM_KEY_SIZE", 0x00000102);
// Parameter name for ups_env_get_parameters;
define("UPS_PARAM_MAX_DATABASES", 0x00000103);
// Parameter name for ups_env_create_db; sets the key type
define("UPS_PARAM_KEY_TYPE", 0x00000104);
// Parameter name for ups_env_open, ups_env_create;
define("UPS_PARAM_LOG_DIRECTORY", 0x00000105);
// Parameter name for ups_env_open, ups_env_create;
define("UPS_PARAM_ENCRYPTION_KEY", 0x00000106);
// Parameter name for ups_env_open, ups_env_create;
define("UPS_PARAM_NETWORK_TIMEOUT_SEC", 0x00000107);
// Parameter name for ups_env_create_db; sets the key size
define("UPS_PARAM_RECORD_SIZE", 0x00000108);
// Parameter name for ups_env_create, ups_env_open;
define("UPS_PARAM_FILE_SIZE_LIMIT", 0x00000109);
// Parameter name for ups_env_create, ups_env_open;
define("UPS_PARAM_POSIX_FADVISE", 0x00000110);
// Parameter name for ups_env_create_db
define("UPS_PARAM_CUSTOM_COMPARE_NAME", 0x00000111);
// Parameter name for ups_env_create_db; sets the record type
define("UPS_PARAM_RECORD_TYPE", 0x00000112);
// Value for UPS_PARAM_POSIX_FADVISE
define("UPS_POSIX_FADVICE_NORMAL", 0);
// Value for UPS_PARAM_POSIX_FADVISE
define("UPS_POSIX_FADVICE_RANDOM", 1);
// Value for unlimited record sizes
define("UPS_RECORD_SIZE_UNLIMITED", 4294967295);
// Value for unlimited key sizes
define("UPS_KEY_SIZE_UNLIMITED", 65535);
// Retrieves the Database/Environment flags
define("UPS_PARAM_FLAGS", 0x00000200);
// Retrieves the filesystem file access mode
define("UPS_PARAM_FILEMODE", 0x00000201);
// Returns the file name of the Environment
define("UPS_PARAM_FILENAME", 0x00000202);
// Returns the name of the database
define("UPS_PARAM_DATABASE_NAME", 0x00000203);
// Retrieve the maximum number of keys per page
define("UPS_PARAM_MAX_KEYS_PER_PAGE", 0x00000204);
// Parameter name for ups_env_create, ups_env_open;
define("UPS_PARAM_JOURNAL_COMPRESSION", 0x00001000);
// Parameter name for ups_env_create_db,
define("UPS_PARAM_RECORD_COMPRESSION", 0x00001001);
// Parameter name for ups_env_create_db,
define("UPS_PARAM_KEY_COMPRESSION", 0x00001002);

// helper macro for disabling compression
define("UPS_COMPRESSOR_NONE", 0);
// selects zlib compression
define("UPS_COMPRESSOR_ZLIB", 1);
// selects google snappy compression
define("UPS_COMPRESSOR_SNAPPY", 2);
// selects lzf compression
define("UPS_COMPRESSOR_LZF", 3);
// uint32 key compression (varbyte)
define("UPS_COMPRESSOR_UINT32_VARBYTE", 5);
// uint32 key compression (BP128)
define("UPS_COMPRESSOR_UINT32_SIMDCOMP", 6);
// uint32 key compression (libfor - Frame Of Reference)
define("UPS_COMPRESSOR_UINT32_FOR", 10);
// uint32 key compression (SIMDFOR - Frame Of Reference w/ SIMD)
define("UPS_COMPRESSOR_UINT32_SIMDFOR", 11);

// Flag for ups_db_close, ups_env_close
//define("UPS_AUTO_CLEANUP", 1); -- not allowed
// Automatically abort all open Transactions (the default)
define("UPS_TXN_AUTO_ABORT", 4);
// Automatically commit all open Transactions
define("UPS_TXN_AUTO_COMMIT", 8);

// Flag for ups_cursor_move
define("UPS_CURSOR_FIRST", 0x0001);
// Flag for ups_cursor_move
define("UPS_CURSOR_LAST", 0x0002);
// Flag for ups_cursor_move
define("UPS_CURSOR_NEXT", 0x0004);
// Flag for ups_cursor_move
define("UPS_CURSOR_PREVIOUS", 0x0008);
// Flag for ups_cursor_move and ups_db_count()
define("UPS_SKIP_DUPLICATES", 0x0010);
// Flag for ups_cursor_move
define("UPS_ONLY_DUPLICATES", 0x0020);
// Cursor 'find' flag 'Less Than': return the nearest match below given key
define("UPS_FIND_LT_MATCH", 0x1000);
// Cursor 'find' flag 'Greater Than': return the nearest match above given key
define("UPS_FIND_GT_MATCH", 0x2000);
// Cursor 'find' flag 'Less or EQual': return the nearest match below given key
define("UPS_FIND_LEQ_MATCH", (0x1000 | 0x4000));
// Cursor 'find' flag 'Greater or Equal': return nearest match above given key
define("UPS_FIND_GEQ_MATCH", (0x2000 | 0x4000));

class Upscaledb {
  /**
   * Returns a human readable status code description
   *
   * @param integer $status The status code
   * @return string The descriptive string
   */
  public static function strerror($status) {
    return "";
  }
}

/**
 * An Upscaledb Environment.
 *
 * This class wraps the native ups_env_t objects, and all the methods
 * starting with "ups_env".
 *
 * @link http://files.upscaledb.com/documentation/html/group__ups__env.html
 */
class Upscaledb {
  /**
   * Creates a new Environment.
   *
   * This method wraps the native ups_env_create function.
   * See the C documentation for a list of flags and parameters
   * that are supported.
   *
   * @link http://files.upscaledb.com/documentation/html/group__ups__env.html#ga30705470fbae862c47512a33564d72a8
   *
   * @param string $filename
   * @param integer $flags
   * @param integer $mode
   * @param array $parameters
   *
   * @return integer
   */
  public function create($name, $flags = 0, $mode = 0,
      array $parameters = []) {
    return UPS_SUCCESS;
  }

  /**
   * Opens an existing Environment.
   *
   * This method wraps the native ups_env_open function.
   * See the C documentation for a list of flags and parameters that
   * are supported.
   *
   * @link http://files.upscaledb.com/documentation/html/group__ups__env.html#ga3f347d5c9b11301c45c3f27624b5f55a
   *
   * @param string $filename
   * @param array $flags
   * @param array $parameters
   *
   * @return integer
   */
  public function open($name, $flags = 0, array $parameters = []) {
    return UPS_SUCCESS;
  }

  /**
   * Creates a new Database in this Environment.
   *
   * This method wraps the native ups_env_create_db function.
   * See the C documentation for a list of flags and parameters that
   * are supported.
   *
   * @link http://files.upscaledb.com/documentation/html/group__ups__env.html#ga38fcbdfa52fe22afe2aa3b3cc7395ea5
   *
   * @param integer $name
   * @param array $flags
   * @param array $parameters
   *
   * @return UpscaledbDatabase if successful
   * @return integer otherwise
   */
  public function create_db($name, $flags = 0, array $parameters = []) {
    return UPS_SUCCESS;
  }

  /**
   * Opens an existing Database from this Environment.
   *
   * This method wraps the native ups_env_open_db function.
   * See the C documentation for a list of flags and parameters that
   * are supported.
   *
   * @link http://files.upscaledb.com/documentation/html/group__ups__env.html#ga2fc332c2e0b4feba84c2e9761c6443a5
   *
   * @param integer $name
   * @param integer $flags
   * @param array $parameters
   *
   * @return UpscaledbDatabase if successful
   * @return integer otherwise
   */
  public function open_db($name, $flags = 0, array $parameters = []) {
    return UPS_SUCCESS;
  }

  /**
   * Renames a database.
   *
   * This method wraps the native ups_env_rename_db function.
   *
   * @link http://files.upscaledb.com/documentation/html/group__ups__env.html#gaeff4a7a4c1363bef3c7f0a178ad8fd5b
   *
   * @param integer $oldname
   * @param integer $newname
   *
   * @return integer
   */
  public function rename_db($oldname, $newname) {
    return UPS_SUCCESS;
  }

  /**
   * Erases (drops) a database.
   *
   * This method wraps the native ups_env_erase_db function. It will fail
   * if the database is currently opened.
   *
   * @link http://files.upscaledb.com/documentation/html/group__ups__env.html#gae5ddd4e578a39dc1927d8a6465bcac2a
   *
   * @param integer $name
   *
   * @return integer
   */
  public function erase_db($name) {
    return UPS_SUCCESS;
  }

  /**
   * Flushes the environment to disk.
   *
   * This method wraps the native ups_env_flush function.
   *
   * @link http://files.upscaledb.com/documentation/html/group__ups__env.html#ga394c0b85da29e3eaca719b67504f755f
   *
   * @param integer $flags
   *
   * @return integer
   */
  public function flush($flags = 0) {
    return UPS_SUCCESS;
  }

  /**
   * Returns a list of all Databases in this Environment
   *
   * This method wraps the native ups_env_get_database_names function.
   *
   * @link http://files.upscaledb.com/documentation/html/group__ups__env.html#ga70ee2ba96afc341acd72a3e7a11090ab
   *
   * @return array of database names if successful
   * @return integer otherwise
   */
  public function get_database_names() {
    return UPS_SUCCESS;
  }

  /**
   * Executes an UQI query
   *
   * This method wraps the native uqi_select_range function.
   *
   * @link http://files.upscaledb.com/documentation/html/upscaledb__uqi_8h.html#acd97c0afe3f02efe842fb2006392747d
   *
   * @param query The query string
   * @param begin A cursor pointing to the beginning of a range. If null
   *    then the query will start at the beginning of the database.
   * @param end A cursor pointing to end beginning of a range. If null
   *    then the query will run till the end of the database.
   *
   * @return UpscaledbResult if successful
   * @return integer otherwise
   */
  public function select_range($query, $begin = null, $end = null) {
    return UPS_SUCCESS;
  }

  /**
   * Closes the Environment.
   *
   * The Environment will be automatically closed when it is destructed.
   * However, this method will fail if you have not yet closed all
   * transactions and databases! It is therefore strongly recommended to
   * close the Environment "manually" (by invoking this function) and check
   * the return code!
   *
   * @param integer $flags
   *
   * @return integer
   */
  public function close($flags) {
    return UPS_SUCCESS;
  }
}

/**
 * An Upscaledb Database.
 *
 * This class wraps the native ups_db_t objects, and all the methods
 * starting with "ups_db".
 *
 * @link http://files.upscaledb.com/documentation/html/group__ups__db.html
 */
class Database {
  /**
   * Inserts a key/record pair.
   *
   * This method wraps the native ups_db_insert function.
   *
   * @link http://files.upscaledb.com/documentation/html/group__ups__database.html#ga6d2f0a2dd9db6d80d215a2dc8b6b2240
   *
   * For record number databases, the $key will be overwritten with the
   * actually inserted record. It is expected that you pass 0:
   *
   * <pre>
   *    $key = 0;
   *    status = $db->insert(null, $key, $record1);
   *    echo("first key: $key\n"); // 1
   *    $key = 0;
   *    status = $db->insert(null, $key, $record2);
   *    echo("second key: $key\n"); // 2
   * </pre>
   *
   * @param UpscaledbTransaction $txn
   * @param mixed $key
   * @param mixed $record
   * @param integer $flags
   *
   * @return integer
   */
  public function insert($txn = null, &$key, $record, $flags) {
    return UPS_SUCCESS;
  }

  /**
   * Finds a record.
   *
   * This method wraps the native ups_db_find function.
   * If you use "approximate matching" (i.e. with UPS_FIND_LT_MATCH), the $key
   * will return the actual key.
   *
   * @link http://files.upscaledb.com/documentation/html/group__ups__database.html#gaeca4f1ebd4289f1d5244a0b1105279a5
   *
   * @param UpscaledbTransaction $txn
   * @param mixed $key
   * @param integer $flags
   *
   * @return mixed if successful
   * @return integer otherwise
   */
  public function find($txn = null, &$key, $flags) {
    return UPS_SUCCESS;
  }

  /**
   * Deletes a key/record pair.
   *
   * This method wraps the native ups_db_erase function.
   *
   * @link http://files.upscaledb.com/documentation/html/group__ups__database.html#gada901528324a1ffba02968dae763e580
   *
   * @param UpscaledbTransaction $txn
   * @param mixed $key
   * @param integer $flags
   *
   * @return integer
   */
  public function erase($txn, $key, $flags = 0) {
    return UPS_SUCCESS;
  }

  /**
   * Closes the Database
   *
   * The Database will be automatically closed when it is destructed.
   * However, this method will fail if you have not yet closed all
   * cursors! It is therefore strongly recommended to close the Database
   * "manually" (by invoking this function) and check the return code!
   *
   * @param integer $flags
   *
   * @return integer
   */
  public function close($flags) {
    return UPS_SUCCESS;
  }
}

/**
 * An Upscaledb Cursor.
 *
 * This class wraps the native ups_cursor_t objects, and all the methods
 * starting with "ups_cursor".
 *
 * @link http://files.upscaledb.com/documentation/html/group__ups__cursor.html
 */
class UpscaledbCursor {
  /**
   * Constructor; creates a cursor
   *
   * This method wraps the native ups_cursor_create function.
   *
   * @link http://files.upscaledb.com/documentation/html/group__ups__cursor.html#gae1e390f43321120c360135f90d3687c5
   *
   * @param UpscaledbDatabase $database
   * @param UpscaledbTransaction $txn
   *
   */
  public function __construct($database, $txn = null) {
  }

  /**
   * Inserts a key/record pair.
   *
   * This method wraps the native ups_cursor_insert function. If successful,
   * the cursor will then point to the newly inserted pair.
   *
   * See the comment at UpscaledbDatabase::insert on how to deal with
   * record number databases.
   *
   * @link http://files.upscaledb.com/documentation/html/group__ups__cursor.html#gab779d0fd20b5df9c92aca9e912fd20b0
   *
   * @param mixed $key
   * @param mixed $record
   * @param integer $flags
   *
   * @return integer
   */
  public function insert(&$key, $record, $flags = 0) {
    return UPS_SUCCESS;
  }

  /**
   * Overwrites the current record.
   *
   * @link http://files.upscaledb.com/documentation/html/group__ups__cursor.html#ga75c375bb54df99570b9865a7b4c8d14e
   *
   * @param mixed $record
   * @param integer $flags
   *
   * @return integer
   */
  public function overwrite($record, $flags = 0) {
    return UPS_SUCCESS;
  }

  /**
   * Looks up a record.
   *
   * This method wraps the native ups_cursor_find function. It points the
   * cursor to the key/record pair, and returns the record.
   *
   * @link http://files.upscaledb.com/documentation/html/group__ups__database.html#gaeca4f1ebd4289f1d5244a0b1105279a5
   *
   * @param mixed $key
   * @param integer $flags
   *
   * @return mixed if successful
   * @return integer otherwise
   */
  public function find($key, $flags = 0) {
    return UPS_SUCCESS;
  }

  /**
   * Deletes the current key/record pair.
   *
   * This method wraps the native ups_cursor_erase function.
   *
   * @link http://files.upscaledb.com/documentation/html/group__ups__cursor.html#gaa1f88fb3a914a15339666519e4a180e1
   *
   * @return integer
   */
  public function erase() {
    return UPS_SUCCESS;
  }

  /**
   * Clones the Cursor.
   *
   * This method wraps the native ups_cursor_clone function.
   *
   * @link http://files.upscaledb.com/documentation/html/group__ups__cursor.html#gaa3573d6c5e8da3667b78025a0b1b8f98
   *
   * @return UpscaledbCursor
   */
  public function clone() {
    return null;
  }

  /**
   * Returns the number of duplicates of the current key.
   *
   * This method wraps the native ups_cursor_get_duplicate_count function.
   * @link http://files.upscaledb.com/documentation/html/group__ups__cursor.html#gac80d65ee801bd81723831d81748ffcfd
   *
   * @return integer > 0 The duplicate count 
   * @return integer < 0 The error code
   */
  public function get_duplicate_count() {
    return null;
  }

  /**
   * Returns the size of the current record.
   *
   * This method wraps the native ups_cursor_get_record_size function.
   * @link http://files.upscaledb.com/documentation/html/group__ups__cursor.html#ga6cc7608a708f217a285e58ab762ef4f3
   *
   * @return integer >= 0 The record size
   * @return integer < 0 The error code
   */
  public function get_record_size() {
    return 0;
  }

  /**
   * Moves the cursor to the specified direction.
   *
   * This method wraps the native ups_cursor_move function.
   * @link http://files.upscaledb.com/documentation/html/group__ups__cursor.html#gaac3c7fa55e3a156bb91381af13bd33c9
   *
   * The @ref flags specify the direction, i.e. UPS_CURSOR_FIRST,
   * UPS_CURSOR_LAST, UPS_CURSOR_NEXT, UPS_CURSOR_PREVIOUS. Duplicates can be
   * skipped if requested (UPS_SKIP_DUPLICATES).
   *
   * @param mixed $key
   * @param mixed $record
   * @param integer $flags
   *
   * @return integer
   */
  public function move(&$key, &$record, $flags) {
    return null;
  }

  /**
   * Closes the Cursor
   *
   * This method wraps the native ups_cursor_close function.
   * The cursor is automatically closed when it is destructed.
   *
   * @link http://files.upscaledb.com/documentation/html/group__ups__cursor.html#ga1b92b930f157ff2d9e0896e9f639085f
   *
   * @return integer
   */
  public function close() {
    return UPS_SUCCESS;
  }
}

/**
 * An Upscaledb Transaction.
 *
 * This class wraps the native ups_txn_t objects, and all the methods
 * starting with "ups_txn".
 *
 * @link http://files.upscaledb.com/documentation/html/group__ups__txn.html
 */
class UpscaledbTransaction {
  /**
   * Constructor; begins a transaction
   *
   * This method wraps the native ups_txn_begin function.
   *
   * @link http://files.upscaledb.com/documentation/html/group__ups__txn.html#gac765d57ce869a6b2a89266fbc20f965e
   *
   * @param UpscaledbEnvironment $env
   * @param UpscaledbTransaction $txn
   *
   */
  public function __construct($database, $txn = null) {
  }

  /**
   * Commits the transaction.
   *
   * This method wraps the native ups_txn_commit function.
   * @link http://files.upscaledb.com/documentation/html/group__ups__txn.html#gada266b6c9cdc883b77adf0268129076c
   *
   * @return integer
   */
  public function commit() {
    return UPS_SUCCESS;
  }

  /**
   * Aborts the transaction.
   *
   * Transactions are automatically aborted when they are destructed.
   *
   * This method wraps the native ups_txn_abort function.
   * @link http://files.upscaledb.com/documentation/html/group__ups__txn.html#ga3a1c2a2a3efa859f9966f53d019598c9
   *
   * @return integer
   */
  public function abort() {
    return UPS_SUCCESS;
  }
}

/**
 * An UQI (Upscaledb Query Interface) Result.
 *
 * This result object is returned by @ref Upscaledb::select_range.
 * This class wraps the native uqi_result_t objects, and all the methods
 * starting with "uqi_result".
 */
class UpscaledbResult {
  /**
   * Returns the number of rows stored in this result object.
   *
   * This method wraps the native uqi_result_get_row_count function.
   * @link http://files.upscaledb.com/documentation/html/upscaledb__uqi_8h.html#ae0eb5ed92b3ab812fafce3177e688d2a
   *
   * @return integer
   */
  public function get_row_count() {
    return 0;
  }

  /**
   * Returns the type of the stored keys.
   *
   * This method wraps the native uqi_result_get_key_type function.
   * @link http://files.upscaledb.com/documentation/html/upscaledb__uqi_8h.html#acb4ee211627ed4819db81f4ea37ae8c3
   *
   * @return integer
   */
  public function get_key_type() {
    return 0;
  }

  /**
   * Returns the type of the stored records.
   *
   * This method wraps the native uqi_result_get_record_type function.
   * @link http://files.upscaledb.com/documentation/html/upscaledb__uqi_8h.html#acf4213a8d8cdbcb0f9aa63617b7486a2
   *
   * @return integer
   */
  public function get_record_type() {
    return 0;
  }

  /**
   * Returns the key of the specified row.
   *
   * This method wraps the native uqi_result_get_key function.
   * @link http://files.upscaledb.com/documentation/html/upscaledb__uqi_8h.html#aeed75ce8b8a3f7cdabfbf72ec031ae3f
   *
   * @return mixed The key data; NULL if $row is out of bounds.
   */
  public function get_key($row) {
    return null;
  }

  /**
   * Returns the record of the specified row.

   * This method wraps the native uqi_result_get_row function.
   * @link http://files.upscaledb.com/documentation/html/upscaledb__uqi_8h.html#a5a9334a1d13fc7c7b4709b06ada85d22
   *
   * @return mixed The record data; NULL if $row is out of bounds.
   */
  public function get_record($row) {
    return null;
  }

  /**
   * Cleans up alloated resources.
   *
   * This method wraps the native uqi_result_close function. It is called
   * automatically whenever the object is destructed.
   *
   * @link http://files.upscaledb.com/documentation/html/upscaledb__uqi_8h.html#a7fa434f1a806ac969499774e81e4c3d2
   *
   * @return integer
   */
  public function close() {
    return UPS_SUCCESS;
  }
}

