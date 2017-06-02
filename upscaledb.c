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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"

#include <ups/upscaledb.h>
#include <ups/upscaledb_uqi.h>

#include "php_upscaledb.h"

#if (PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 4)
# define UPSCALEDB_SAFE_MODE_CHECK(file) || (PG(safe_mode) && !php_checkuid((file), "rb+", CHECKUID_CHECK_MODE_PARAM))
#else
# define UPSCALEDB_SAFE_MODE_CHECK(file)
#endif

#define UPSCALEDB_CHECK_OPEN_BASEDIR(file) \
    if (php_check_open_basedir((file) TSRMLS_CC) UPSCALEDB_SAFE_MODE_CHECK((file))){ \
        RETURN_FALSE; \
    }

#define PHP_UPSCALEDB_ERROR_ENV_CLOSED 1
#define PHP_UPSCALEDB_ERROR_DB_CLOSED 1

#define UPSCALEDB_CHECK_ENV_NOT_CLOSED(db_object) \
    if ((db_object)->env == NULL) { \
        zend_throw_exception(php_upscaledb_ce_UpscaledbException, "Can not operate on closed env", PHP_UPSCALEDB_ERROR_ENV_CLOSED TSRMLS_CC); \
        return; \
    }

#define UPSCALEDB_CHECK_DB_NOT_CLOSED(db_object) \
    if ((db_object)->db == NULL) { \
        zend_throw_exception(php_upscaledb_ce_UpscaledbException, "Can not operate on closed db", PHP_UPSCALEDB_ERROR_DB_CLOSED TSRMLS_CC); \
        return; \
    }

#ifndef PHP_FE_END
# define PHP_FE_END { NULL, NULL, NULL, 0, 0 }
#endif

#if ZEND_MODULE_API_NO < 20090626
# define Z_ADDREF_P(arg) ZVAL_ADDREF(arg)
# define Z_ADDREF_PP(arg) ZVAL_ADDREF(*(arg))
# define Z_DELREF_P(arg) ZVAL_DELREF(arg)
# define Z_DELREF_PP(arg) ZVAL_DELREF(*(arg))
#endif

#ifndef zend_parse_parameters_none
# define zend_parse_parameters_none() zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "")
#endif

#if ZEND_MODULE_API_NO >= 20100525
#define init_properties(intern) object_properties_init(&intern->std, class_type)
#else
#define init_properties(intern) do { \
    zval *tmp; \
    zend_hash_copy(intern->std.properties, \
    &class_type->default_properties, (copy_ctor_func_t) zval_add_ref,  \
    (void *) &tmp, sizeof(zval *)); \
} while(0)
#endif

/* PHP-UpscaleDB MAGIC identifier don't change this */
#define PHP_UPSCALEDB_CUSTOM_COMPARATOR_NAME "php_upscaledb.custom_comparator"

#define php_upscaledb_obj_new(obj, class_type)                    \
  zend_object_value retval;                                        \
  obj *intern;                                                    \
                                                                \
  intern = (obj *)emalloc(sizeof(obj));                           \
  memset(intern, 0, sizeof(obj));                                  \
                                                                \
  zend_object_std_init(&intern->std, class_type TSRMLS_CC);     \
  init_properties(intern);                                        \
                                                                \
  retval.handle = zend_objects_store_put(intern,                \
     (zend_objects_store_dtor_t) zend_objects_destroy_object,    \
     php_##obj##_free, NULL TSRMLS_CC);                            \
  retval.handlers = &upscaledb_default_handlers;                \
  return retval;

#define UPSCALEDB_CHECK_ERROR(err) \
    if ((err) != NULL) { \
        zend_throw_exception(php_upscaledb_ceUpscaledbException, err, 0 TSRMLS_CC); \
        free(err); \
        return; \
    }

/* {{{ upscaledb_functions[]
 */
const zend_function_entry upscaledb_functions[] = {
    PHP_FE_END    /* Must be the last line in upscaledb_functions[] */
};
/* }}} */

/* {{{ upscaledb_module_entry
 */
zend_module_entry upscaledb_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "upscaledb",
    upscaledb_functions,
    PHP_MINIT(upscaledb),
    PHP_MSHUTDOWN(upscaledb),
    NULL,
    NULL,
    PHP_MINFO(upscaledb),
#if ZEND_MODULE_API_NO >= 20010901
    PHP_UPSCALEDB_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_UPSCALEDB
ZEND_GET_MODULE(upscaledb)
#endif

/* Handlers */
static zend_object_handlers upscaledb_default_handlers;
static zend_object_handlers upscaledb_object_handlers;

/* Class entries */
zend_class_entry *php_upscaledb_ce_UpscaledbException;
zend_class_entry *php_upscaledb_class_entry;
zend_class_entry *php_upscaledb_db_class_entry;
zend_class_entry *php_upscaledb_cursor_class_entry;
zend_class_entry *php_upscaledb_result_class_entry;
zend_class_entry *php_upscaledb_txn_class_entry;

/* Objects */
typedef struct {
  zend_object std;
  ups_env_t *env;
} upscaledb_object;

typedef struct {
  zend_object std;
  ups_db_t *db;
  zval *zenv;
  uint32_t key_type;
  uint32_t record_type;
  uint32_t flags;
} upscaledb_db_object;

typedef struct {
  zend_object std;
  ups_cursor_t *cursor;
  upscaledb_db_object *db;
  zval *zdb;
} upscaledb_cursor_object;

typedef struct {
  zend_object std;
  uqi_result_t *result;
  uint32_t key_type;
  uint32_t record_type;
  uint32_t row_count;
} upscaledb_result_object;

typedef struct {
  zend_object std;
  ups_txn_t *txn;
  zval *zenv;
} upscaledb_txn_object;

static void
php_upscaledb_object_free(void *object TSRMLS_DC)
{
  upscaledb_object *obj = (upscaledb_object *)object;

  if (obj->env) {
    ups_env_close(obj->env, 0);
    obj->env = 0;
  }

  zend_objects_free_object_storage((zend_object *)object TSRMLS_CC);
}

static void
php_upscaledb_db_object_free(void *object TSRMLS_DC)
{
  upscaledb_db_object *obj = (upscaledb_db_object *)object;

  if (obj->db) {
    ups_db_close(obj->db, 0);
    obj->db = 0;
  }

  if (obj->zenv) {
    zval_ptr_dtor(&obj->zenv);
    obj->zenv = 0;
  }

  zend_objects_free_object_storage((zend_object *)object TSRMLS_CC);
}

static void
php_upscaledb_cursor_object_free(void *object TSRMLS_DC)
{
  upscaledb_cursor_object *obj = (upscaledb_cursor_object *)object;

  if (obj->cursor) {
    ups_cursor_close(obj->cursor);
    obj->cursor = 0;
  }

  if (obj->zdb) {
    zval_ptr_dtor(&obj->zdb);
    obj->zdb = 0;
  }

  zend_objects_free_object_storage((zend_object *)object TSRMLS_CC);
}

static void
php_upscaledb_result_object_free(void *object TSRMLS_DC)
{
  upscaledb_result_object *obj = (upscaledb_result_object *)object;

  if (obj->result) {
    uqi_result_close(obj->result);
    obj->result = 0;
  }

  zend_objects_free_object_storage((zend_object *)object TSRMLS_CC);
}

static void
php_upscaledb_txn_object_free(void *object TSRMLS_DC)
{
  upscaledb_txn_object *obj = (upscaledb_txn_object *)object;

  if (obj->txn) {
    ups_txn_abort(obj->txn, 0);
    obj->txn = 0;
  }

  if (obj->zenv) {
    zval_ptr_dtor(&obj->zenv);
    obj->zenv = 0;
  }

  zend_objects_free_object_storage((zend_object *)object TSRMLS_CC);
}

static zend_object_value
php_upscaledb_object_new(zend_class_entry *class_type TSRMLS_DC)
{
  php_upscaledb_obj_new(upscaledb_object, class_type);
}

static zend_object_value
php_upscaledb_db_object_new(zend_class_entry *class_type TSRMLS_DC)
{
  php_upscaledb_obj_new(upscaledb_db_object, class_type);
}

static zend_object_value
php_upscaledb_cursor_object_new(zend_class_entry *class_type TSRMLS_DC)
{
  php_upscaledb_obj_new(upscaledb_cursor_object, class_type);
}

static zend_object_value
php_upscaledb_result_object_new(zend_class_entry *class_type TSRMLS_DC)
{
  php_upscaledb_obj_new(upscaledb_result_object, class_type);
}

static zend_object_value
php_upscaledb_txn_object_new(zend_class_entry *class_type TSRMLS_DC)
{
  php_upscaledb_obj_new(upscaledb_txn_object, class_type);
}

/* arg info */
ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_construct, 0, 0, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_create, 0, 0, 1)
  ZEND_ARG_INFO(0, name)
  ZEND_ARG_INFO(0, flags)
  ZEND_ARG_INFO(0, mode)
  ZEND_ARG_INFO(0, parameters)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_open, 0, 0, 1)
  ZEND_ARG_INFO(0, name)
  ZEND_ARG_INFO(0, flags)
  ZEND_ARG_INFO(0, parameters)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_create_db, 0, 0, 1)
  ZEND_ARG_INFO(0, name)
  ZEND_ARG_INFO(0, flags)
  ZEND_ARG_INFO(0, parameters)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_open_db, 0, 0, 1)
  ZEND_ARG_INFO(0, name)
  ZEND_ARG_INFO(0, flags)
  ZEND_ARG_INFO(0, parameters)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_erase_db, 0, 0, 1)
  ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_rename_db, 0, 0, 2)
  ZEND_ARG_INFO(0, oldname)
  ZEND_ARG_INFO(0, newname)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_flush, 0, 0, 0)
  ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_select_range, 0, 0, 1)
  ZEND_ARG_INFO(0, query)
  ZEND_ARG_INFO(0, begin)
  ZEND_ARG_INFO(0, end)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_get_database_names, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_close, 0, 0, 0)
  ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_strerror, 0, 0, 0)
  ZEND_ARG_INFO(0, status)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_db_construct, 0, 0, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_db_insert, 0, 0, 2)
  ZEND_ARG_INFO(0, key)
  ZEND_ARG_INFO(0, value)
  ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_db_find, 0, 0, 1)
  ZEND_ARG_INFO(0, key)
  ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_db_erase, 0, 0, 1)
  ZEND_ARG_INFO(0, key)
  ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_db_close, 0, 0, 0)
  ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_cursor_construct, 0, 0, 0)
  ZEND_ARG_INFO(0, db)
  ZEND_ARG_INFO(0, txn)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_cursor_insert, 0, 0, 2)
  ZEND_ARG_INFO(0, key)
  ZEND_ARG_INFO(0, value)
  ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_cursor_overwrite, 0, 0, 1)
  ZEND_ARG_INFO(0, value)
  ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_cursor_find, 0, 0, 1)
  ZEND_ARG_INFO(0, key)
  ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_cursor_erase, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_cursor_clone, 0, 0, 1)
  ZEND_ARG_INFO(0, other)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_cursor_move, 0, 0, 0)
  ZEND_ARG_INFO(0, key)
  ZEND_ARG_INFO(0, value)
  ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_cursor_get_duplicate_count, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_cursor_get_record_size, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_cursor_close, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_txn_construct, 0, 0, 1)
  ZEND_ARG_INFO(0, env)
  ZEND_ARG_INFO(0, name)
  ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_txn_commit, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_txn_abort, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_result_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_result_get_row_count, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_result_get_key_type, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_result_get_record_type, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_result_get_key, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_result_get_record, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_upscaledb_result_close, 0, 0, 0)
ZEND_END_ARG_INFO()

/* Methods */

/* {{{ php_upscaledb_class_methods */
static zend_function_entry php_upscaledb_class_methods[] = {
  PHP_ME(Upscaledb, __construct, arginfo_upscaledb_construct,
                  ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(Upscaledb, create, arginfo_upscaledb_create, ZEND_ACC_PUBLIC)
  PHP_ME(Upscaledb, open, arginfo_upscaledb_open, ZEND_ACC_PUBLIC)
  PHP_ME(Upscaledb, create_db, arginfo_upscaledb_create_db, ZEND_ACC_PUBLIC)
  PHP_ME(Upscaledb, open_db, arginfo_upscaledb_open_db, ZEND_ACC_PUBLIC)
  PHP_ME(Upscaledb, erase_db, arginfo_upscaledb_erase_db, ZEND_ACC_PUBLIC)
  PHP_ME(Upscaledb, rename_db, arginfo_upscaledb_rename_db, ZEND_ACC_PUBLIC)
  PHP_ME(Upscaledb, flush, arginfo_upscaledb_flush, ZEND_ACC_PUBLIC)
  PHP_ME(Upscaledb, select_range, arginfo_upscaledb_select_range,
                  ZEND_ACC_PUBLIC)
  PHP_ME(Upscaledb, get_database_names, arginfo_upscaledb_get_database_names,
                  ZEND_ACC_PUBLIC)
  PHP_ME(Upscaledb, close, arginfo_upscaledb_close, ZEND_ACC_PUBLIC)
  PHP_ME(Upscaledb, strerror, arginfo_upscaledb_strerror,
                  ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
  PHP_FE_END
};
/* }}} */

/* {{{ php_upscaledb_db_class_methods */
static zend_function_entry php_upscaledb_db_class_methods[] = {
  PHP_ME(UpscaledbDatabase, __construct, arginfo_upscaledb_db_construct,
                  ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(UpscaledbDatabase, insert, arginfo_upscaledb_db_insert,
                  ZEND_ACC_PUBLIC)
  PHP_ME(UpscaledbDatabase, find, arginfo_upscaledb_db_find, ZEND_ACC_PUBLIC)
  PHP_ME(UpscaledbDatabase, erase, arginfo_upscaledb_db_erase, ZEND_ACC_PUBLIC)
  PHP_ME(UpscaledbDatabase, close, arginfo_upscaledb_db_close, ZEND_ACC_PUBLIC)
  PHP_FE_END
};
/* }}} */

/* {{{ php_upscaledb_cursor_class_methods */
static zend_function_entry php_upscaledb_cursor_class_methods[] = {
  PHP_ME(UpscaledbCursor, __construct, arginfo_upscaledb_cursor_construct,
                  ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(UpscaledbCursor, insert, arginfo_upscaledb_cursor_insert,
                  ZEND_ACC_PUBLIC)
  PHP_ME(UpscaledbCursor, overwrite, arginfo_upscaledb_cursor_overwrite,
                  ZEND_ACC_PUBLIC)
  PHP_ME(UpscaledbCursor, find, arginfo_upscaledb_cursor_find,
                  ZEND_ACC_PUBLIC)
  PHP_ME(UpscaledbCursor, erase, arginfo_upscaledb_cursor_erase,
                  ZEND_ACC_PUBLIC)
  PHP_ME(UpscaledbCursor, clone, arginfo_upscaledb_cursor_clone,
                  ZEND_ACC_PUBLIC)
  PHP_ME(UpscaledbCursor, move, arginfo_upscaledb_cursor_move,
                  ZEND_ACC_PUBLIC)
  PHP_ME(UpscaledbCursor, get_duplicate_count,
                  arginfo_upscaledb_cursor_get_duplicate_count, ZEND_ACC_PUBLIC)
  PHP_ME(UpscaledbCursor, get_record_size,
                  arginfo_upscaledb_cursor_get_record_size, ZEND_ACC_PUBLIC)
  PHP_ME(UpscaledbCursor, close, arginfo_upscaledb_cursor_close,
                  ZEND_ACC_PUBLIC)
  PHP_FE_END
};
/* }}} */

/* {{{ php_upscaledb_txn_class_methods */
static zend_function_entry php_upscaledb_txn_class_methods[] = {
  PHP_ME(UpscaledbTransaction, __construct, arginfo_upscaledb_txn_construct,
                  ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(UpscaledbTransaction, abort, arginfo_upscaledb_txn_abort,
                  ZEND_ACC_PUBLIC)
  PHP_ME(UpscaledbTransaction, commit, arginfo_upscaledb_txn_commit,
                  ZEND_ACC_PUBLIC)
  PHP_FE_END
};
/* }}} */

/* {{{ php_upscaledb_result_class_methods */
static zend_function_entry php_upscaledb_result_class_methods[] = {
  PHP_ME(UpscaledbResult, __construct, arginfo_upscaledb_result_construct,
                  ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(UpscaledbResult, get_row_count, arginfo_upscaledb_result_get_row_count,
                  ZEND_ACC_PUBLIC)
  PHP_ME(UpscaledbResult, get_key_type, arginfo_upscaledb_result_get_key_type,
                  ZEND_ACC_PUBLIC)
  PHP_ME(UpscaledbResult, get_record_type,
                  arginfo_upscaledb_result_get_record_type, ZEND_ACC_PUBLIC)
  PHP_ME(UpscaledbResult, get_key, arginfo_upscaledb_result_get_key,
                  ZEND_ACC_PUBLIC)
  PHP_ME(UpscaledbResult, get_record, arginfo_upscaledb_result_get_record,
                  ZEND_ACC_PUBLIC)
  PHP_ME(UpscaledbResult, close, arginfo_upscaledb_result_close,
                  ZEND_ACC_PUBLIC)
  PHP_FE_END
};
/* }}} */

static void
upscaledb_custom_comparator_destructor(void *stat)
{
  zval *callable = (zval *)stat;
  zval_ptr_dtor(&callable);
}

static ups_status_t
php_array_to_parameters(HashTable *pht, ups_parameter_t *params,
                int params_capacity, int *params_length /* in/out */)
{
  HashPosition pos;
  zval **data;
  int p = 0;

  for (zend_hash_internal_pointer_reset_ex(pht, &pos);
                  zend_hash_get_current_data_ex(pht, (void **)&data, &pos)
                        == SUCCESS;
                  zend_hash_move_forward_ex(pht, &pos)) {
    char *str_index;
    uint str_length;
    ulong num_index;

    if (zend_hash_get_current_key_ex(pht, &str_index, &str_length,
                            &num_index, 0, &pos) != HASH_KEY_IS_LONG)
      return UPS_INV_PARAMETER;

    /* Do something with data */
    params[p].name = num_index;

    switch (num_index) {
      /* parameters with a numeric value */
      case UPS_PARAM_JOURNAL_COMPRESSION:
      case UPS_PARAM_RECORD_COMPRESSION:
      case UPS_PARAM_KEY_COMPRESSION:
      case UPS_PARAM_CACHE_SIZE:
      case UPS_PARAM_PAGE_SIZE:
      case UPS_PARAM_FILE_SIZE_LIMIT:
      case UPS_PARAM_KEY_SIZE:
      case UPS_PARAM_RECORD_SIZE:
      case UPS_PARAM_KEY_TYPE:
      case UPS_PARAM_RECORD_TYPE:
      case UPS_PARAM_MAX_DATABASES:
      case UPS_PARAM_NETWORK_TIMEOUT_SEC:
        if ((*data)->type != IS_LONG)
          return UPS_INV_PARAMETER;
        params[p].value = (uint64_t)Z_LVAL_P(*data);
        break;

      /* parameters with a string value */
      case UPS_PARAM_LOG_DIRECTORY:
      case UPS_PARAM_ENCRYPTION_KEY:
        if ((*data)->type != IS_STRING)
          return UPS_INV_PARAMETER;
        params[p].value = (uint64_t)Z_STRVAL_P(*data);
        break;

      /* read-only parameters without value */
      case UPS_PARAM_FLAGS:
      case UPS_PARAM_FILEMODE:
      case UPS_PARAM_FILENAME:
      case UPS_PARAM_DATABASE_NAME:
      case UPS_PARAM_MAX_KEYS_PER_PAGE:
        params[p].value = 0;
        break;
      default:
        return UPS_INV_PARAMETER;
    }

    p++;
    if (p == params_capacity)
      return UPS_LIMITS_REACHED;
  }

  // the terminating element
  params[p].name = 0;
  params[p].value = 0;

  *params_length = p;
  return 0;
}

/* {{{ proto Upscaledb Upscaledb::__construct() */
PHP_METHOD(Upscaledb, __construct)
{
}
/* }}} */

/*  {{{ proto integer Upscaledb::create(string $name, integer $flags = 0,
 *                  integer $mode = 0, array $parameters = [])
    Creates a new Environment */
PHP_METHOD(Upscaledb, create)
{
  char *name = 0;
  int name_length = 0;
  long flags = 0;
  long mode = 0;
  int params_length = 0;
  ups_parameter_t params[32]; // should be enough
  ups_db_t *db = 0;
  HashTable *pht = 0;
  upscaledb_object *obj;
  ups_status_t st;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|llh", &name,
                          &name_length, &flags, &mode, &pht) == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  /* convert pht hash table to parameters */
  if (pht) {
    st = php_array_to_parameters(pht, params, 32, &params_length);
    if (st)
      RETURN_LONG(st);
  }

  obj = (upscaledb_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  if (obj->env)
    RETURN_LONG(UPS_ALREADY_INITIALIZED);

  st = ups_env_create(&obj->env, name, (uint32_t)flags, (uint32_t)mode,
                  params_length ? &params[0] : 0);
  RETURN_LONG(st);
}
/* }}} */

/* {{{ proto integer Upscaledb::open(string $name, integer $flags = 0,
 *                   array $parameters = [])
    Opens an existing Environment */
PHP_METHOD(Upscaledb, open)
{
  char *name = 0;
  int name_length = 0;
  long flags = 0;
  int params_length = 0;
  ups_parameter_t params[32]; // should be enough
  ups_db_t *db = 0;
  HashTable *pht = 0;
  upscaledb_object *obj;
  ups_status_t st;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|lh", &name,
                          &name_length, &flags, &pht) == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  /* convert pht hash table to parameters */
  if (pht) {
    st = php_array_to_parameters(pht, params, 32, &params_length);
    if (st)
      RETURN_LONG(st);
  }

  obj = (upscaledb_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  if (obj->env)
    RETURN_LONG(UPS_ALREADY_INITIALIZED);

  st = ups_env_open(&obj->env, name, (uint32_t)flags,
                  params_length ? &params[0] : 0);
  RETURN_LONG(st);
}
/* }}} */

/* {{{ proto UpscaledbDatabase|integer Upscaledb::create_db(integer $name,
 *                  integer $flags = 0, array $parameters = [])
    Creates a new Database */
PHP_METHOD(Upscaledb, create_db)
{
  long name = 0;
  long flags = 0;
  int params_length = 0;
  ups_parameter_t params[32]; // should be enough
  ups_db_t *db = 0;
  HashTable *pht = 0;
  upscaledb_object *obj;
  upscaledb_db_object *db_obj;
  ups_status_t st;
  int i;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|lh", &name,
                          &flags, &pht) == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  /* convert pht hash table to parameters */
  if (pht) {
    st = php_array_to_parameters(pht, params, 32, &params_length);
    if (st)
      RETURN_LONG(st);
  }

  obj = (upscaledb_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

  st = ups_env_create_db(obj->env, &db, (uint16_t)name,
                  (uint32_t)flags, params_length ? &params[0] : 0);
  if (st)
    RETURN_LONG(st);

  object_init_ex(return_value, php_upscaledb_db_class_entry);
  db_obj = (upscaledb_db_object *)zend_object_store_get_object(return_value TSRMLS_CC);
  db_obj->db = db;
  db_obj->flags = flags;
  db_obj->zenv = getThis();
  Z_ADDREF_P(db_obj->zenv);

  for (i = 0; i < params_length; i++) {
    if (params[i].name == UPS_PARAM_KEY_TYPE)
      db_obj->key_type = (uint32_t)params[i].value;
    else if (params[i].name == UPS_PARAM_RECORD_TYPE)
      db_obj->record_type = (uint32_t)params[i].value;
  }

  if (flags & UPS_RECORD_NUMBER32)
    db_obj->key_type = UPS_TYPE_UINT32;
  else if (flags & UPS_RECORD_NUMBER64)
    db_obj->key_type = UPS_TYPE_UINT64;
}
/* }}} */

/* {{{ proto UpscaledbDatabase|integer Upscaledb::open_db(integer $name,
 *                  integer $flags = 0, array $parameters = [])
  Opens an existing Database */
PHP_METHOD(Upscaledb, open_db)
{
  long name = 0;
  long flags = 0;
  int params_length = 0;
  ups_parameter_t params[32]; // should be enough
  ups_db_t *db = 0;
  HashTable *pht = 0;
  upscaledb_object *obj;
  upscaledb_db_object *db_obj;
  ups_status_t st;
  int i;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|lh", &name,
                          &flags, &pht) == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  /* convert pht hash table to parameters */
  if (pht) {
    st = php_array_to_parameters(pht, params, 32, &params_length);
    if (st)
      RETURN_LONG(st);
  }

  obj = (upscaledb_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

  st = ups_env_open_db(obj->env, &db, (uint16_t)name,
                  (uint32_t)flags, params_length ? &params[0] : 0);
  if (st)
    RETURN_LONG(st);

  // get the database flags
  ups_parameter_t p[] = {
    {UPS_PARAM_FLAGS, 0},
    {UPS_PARAM_KEY_TYPE, 0},
    {UPS_PARAM_RECORD_TYPE, 0},
    {0, 0}
  };
  st = ups_db_get_parameters(db, &p[0]);
  if (st)
    RETURN_LONG(st);

  object_init_ex(return_value, php_upscaledb_db_class_entry);
  db_obj = (upscaledb_db_object *)zend_object_store_get_object(return_value TSRMLS_CC);
  db_obj->db = db;
  db_obj->flags = (uint32_t)p[0].value;
  db_obj->zenv = getThis();
  Z_ADDREF_P(db_obj->zenv);

  db_obj->key_type = (uint32_t) p[1].value;
  db_obj->record_type = (uint32_t) p[2].value;

  if (db_obj->flags & UPS_RECORD_NUMBER32)
    db_obj->key_type = UPS_TYPE_UINT32;
  else if (db_obj->flags & UPS_RECORD_NUMBER64)
    db_obj->key_type = UPS_TYPE_UINT64;
}
/* }}} */

/* {{{ proto integer Upscaledb::erase_db(integer $name)
  Deletes a database from the environment */
PHP_METHOD(Upscaledb, erase_db)
{
  long name = 0;
  upscaledb_object *obj;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &name)
                  == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  obj = (upscaledb_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  ups_status_t st = ups_env_erase_db(obj->env, (uint16_t)name, 0);
  RETURN_LONG(st);
}
/* }}} */

/* {{{ proto integer Upscaledb::rename_db($oldname, $newname)
  Renames a database in the environment. */
PHP_METHOD(Upscaledb, rename_db)
{
  long oldname = 0;
  long newname = 0;
  upscaledb_object *obj;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &oldname, &newname)
                  == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  obj = (upscaledb_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  ups_status_t st = ups_env_rename_db(obj->env, (uint16_t)oldname,
                        (uint16_t)newname, 0);
  RETURN_LONG(st);
}
/* }}} */

/* {{{ proto integer Upscaledb::flush(integer $flags = 0)
  Flushes the environment. */
PHP_METHOD(Upscaledb, flush)
{
  long flags = 0;
  upscaledb_object *obj;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &flags)
                  == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  obj = (upscaledb_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  ups_status_t st = ups_env_flush(obj->env, (uint32_t)flags);
  RETURN_LONG(st);
}
/* }}} */

/* {{{ proto integer Upscaledb::select_range(string $query, UpscaledbCursor
                $begin = null, UpscaledbCursor $end = null) */
PHP_METHOD(Upscaledb, select_range)
{
  char *query = 0;
  int query_length = 0;
  zval *begin = 0;
  zval *end = 0;
  upscaledb_object *obj;
  upscaledb_cursor_object *begin_obj = 0;
  upscaledb_cursor_object *end_obj = 0;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|OO", &query,
              &query_length, &begin, php_upscaledb_cursor_class_entry,
              &end, php_upscaledb_cursor_class_entry)
                  == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  obj = (upscaledb_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  if (begin)
    begin_obj = (upscaledb_cursor_object *)zend_object_store_get_object(begin TSRMLS_CC);
  if (end)
    end_obj = (upscaledb_cursor_object *)zend_object_store_get_object(end TSRMLS_CC);

  uqi_result_t *result;
  ups_status_t st = uqi_select_range(obj->env, query,
                        begin_obj ? begin_obj->cursor : 0, 
                        end_obj ? end_obj->cursor : 0, &result);
  if (st)
    RETURN_LONG(st);

  upscaledb_result_object *ro;
  object_init_ex(return_value, php_upscaledb_result_class_entry);
  ro = (upscaledb_result_object *)zend_object_store_get_object(return_value TSRMLS_CC);
  ro->result = result;
  ro->key_type = uqi_result_get_key_type(result);
  ro->record_type = uqi_result_get_record_type(result);
  ro->row_count = uqi_result_get_row_count(result);
}
/* }}} */

/* {{{ proto array Upscaledb::get_database_names()
  Returns an array with all database names. */
PHP_METHOD(Upscaledb, get_database_names)
{
  upscaledb_object *obj;
  uint16_t names[1024];
  int names_length = sizeof(names) / sizeof(uint16_t);
  zval *arr;
  int i;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "")
                  == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  obj = (upscaledb_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  ups_status_t st = ups_env_get_database_names(obj->env, names, (uint32_t *) &names_length);
  if (st)
    RETURN_LONG(st);

  MAKE_STD_ZVAL(arr);
  array_init(arr);
  for (i = 0; i < names_length; i++)
    add_next_index_long(arr, names[i]);

  RETVAL_ZVAL(arr, 1, 0);
}

/*  {{{ proto integer Upscaledb::close(integer $flags = 0)
  Closes the environment */
PHP_METHOD(Upscaledb, close)
{
  long flags = 0;
  upscaledb_object *obj;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &flags)
                  == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  obj = (upscaledb_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  if (obj->env) {
    ups_status_t st = ups_env_close(obj->env, (uint32_t)flags);
    if (st)
      RETURN_LONG(st);
    obj->env = 0;
  }

  RETURN_LONG(0);
}

/*  {{{ proto string Upscaledb::strerror(integer $status)
  Returns error description as string */
PHP_METHOD(Upscaledb, strerror)
{
  long status = 0;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &status)
                  == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  const char *str = ups_strerror(status);
  RETURN_STRING(str ? str : "", 1);
}
/* }}} */

/* {{{ proto UpscaledbDatabase UpscaledbDatabase::__construct() */
PHP_METHOD(UpscaledbDatabase, __construct)
{
}

static inline ups_status_t
zval_to_key(uint32_t key_type, zval *zk, ups_key_t *key, uint8_t *arena)
{
  if (zk->type == IS_STRING) {
    key->data = zk->value.str.val;
    key->size = zk->value.str.len;
    return 0;
  }

  if (zk->type == IS_LONG) {
    switch (key_type) {
      case UPS_TYPE_UINT8:
        *(uint32_t *)arena = (uint8_t)zk->value.lval;
        key->data = arena;
        key->size = 1;
        return 0;
      case UPS_TYPE_UINT16:
        *(uint32_t *)arena = (uint16_t)zk->value.lval;
        key->data = arena;
        key->size = 2;
        return 0;
      case UPS_TYPE_UINT32:
        *(uint32_t *)arena = (uint32_t)zk->value.lval;
        key->data = arena;
        key->size = 4;
        return 0;
      case UPS_TYPE_UINT64:
      case UPS_TYPE_CUSTOM:
      case UPS_TYPE_BINARY:
        key->data = &zk->value.lval;
        key->size = 8;
        return 0;
    }
  }

  if (zk->type == IS_DOUBLE) {
    switch (key_type) {
      case UPS_TYPE_REAL32:
        *(float *)arena = (float)zk->value.dval;
        key->data = arena;
        key->size = sizeof(float);
        return 0;
      case UPS_TYPE_REAL64:
      case UPS_TYPE_CUSTOM:
      case UPS_TYPE_BINARY:
        key->data = &zk->value.dval;
        key->size = sizeof(zk->value.dval);
        return 0;
    }
  }

  if (zk->type == IS_NULL) {
    key->data = 0;
    key->size = 0;
    return 0;
  }

  if (zk->type == IS_BOOL) {
    key->data = &zk->value.lval;
    key->size = 1;
    return 0;
  }

  return UPS_INV_PARAMETER;
}

static inline ups_status_t
zval_to_record(uint32_t record_type, zval *zr, ups_record_t *rec,
                uint8_t *arena)
{
  if (zr->type == IS_STRING) {
    rec->data = zr->value.str.val;
    rec->size = zr->value.str.len;
    return 0;
  }

  if (zr->type == IS_LONG) {
    switch (record_type) {
      case UPS_TYPE_UINT8:
        rec->data = &zr->value.lval;
        rec->size = 1;
        return 0;
      case UPS_TYPE_UINT16:
        rec->data = &zr->value.lval;
        rec->size = 2;
        return 0;
      case UPS_TYPE_UINT32:
        rec->data = &zr->value.lval;
        rec->size = 4;
        return 0;
      case UPS_TYPE_UINT64:
      case UPS_TYPE_CUSTOM:
      case UPS_TYPE_BINARY:
        rec->data = &zr->value.lval;
        rec->size = 8;
        return 0;
    }
  }

  if (zr->type == IS_DOUBLE) {
    switch (record_type) {
      case UPS_TYPE_REAL32:
        *(float *)arena = (float)zr->value.dval;
        rec->data = arena;
        rec->size = sizeof(float);
        return 0;
      case UPS_TYPE_REAL64:
      case UPS_TYPE_CUSTOM:
      case UPS_TYPE_BINARY:
        rec->data = &zr->value.dval;
        rec->size = sizeof(zr->value.dval);
        return 0;
    }
  }

  if (zr->type == IS_NULL) {
    rec->data = 0;
    rec->size = 0;
    return 0;
  }

  if (zr->type == IS_BOOL) {
    rec->data = &zr->value.lval;
    rec->size = 1;
    return 0;
  }

  return UPS_INV_PARAMETER;
}

static inline void
item_to_zval(uint32_t type, void *data, uint32_t size, zval *zv)
{
  switch (type) {
    case UPS_TYPE_UINT8:
      zv->type = IS_LONG;
      Z_LVAL_P(zv) = *(uint8_t *)data; /* bool uses lval */
      break;
    case UPS_TYPE_UINT16:
      zv->type = IS_LONG;
      Z_LVAL_P(zv) = *(uint16_t *)data;
      break;
    case UPS_TYPE_UINT32:
      zv->type = IS_LONG;
      Z_LVAL_P(zv) = *(uint32_t *)data;
      break;
    case UPS_TYPE_UINT64:
      zv->type = IS_LONG;
      Z_LVAL_P(zv) = *(uint64_t *)data;
      break;
    case UPS_TYPE_REAL32:
      zv->type = IS_DOUBLE;
      Z_DVAL_P(zv) = *(float *)data;
      break;
    case UPS_TYPE_REAL64:
      zv->type = IS_DOUBLE;
      Z_DVAL_P(zv) = *(double *)data;
      break;
    default:
      zv->type = IS_STRING;
      ZVAL_STRINGL(zv, (char *)data, size, 1);
      break;
  }
}

/* {{{ proto integer UpscaledbDatabase::insert(mixed $txn, mixed $key,
 * mixed $value, integer flags = 0) */
PHP_METHOD(UpscaledbDatabase, insert)
{
  zval *ztxn = 0;
  zval *zk = 0;
  zval *zr = 0;
  long flags = 0;
  upscaledb_db_object *obj;
  upscaledb_txn_object *txn_obj = 0;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O!zz|l",
                          &ztxn, php_upscaledb_txn_class_entry, &zk,
                          &zr, &flags)
                  == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  obj = (upscaledb_db_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  if (ztxn)
    txn_obj = (upscaledb_txn_object *)zend_object_store_get_object(ztxn TSRMLS_CC);

  ups_status_t st;
  uint8_t key_arena[16];
  ups_key_t key = {0};
  if ((obj->flags & (UPS_RECORD_NUMBER32 | UPS_RECORD_NUMBER64)) == 0) {
    st = zval_to_key(obj->key_type, zk, &key, key_arena);
    if (st)
      RETURN_LONG(st);
  }
  else if (zk->type != IS_NULL && Z_LVAL_P(zk) != 0)
    RETURN_LONG(UPS_INV_PARAMETER);

  uint8_t record_arena[16];
  ups_record_t record = {0};
  st = zval_to_record(obj->record_type, zr, &record, record_arena);
  if (st)
    RETURN_LONG(st);

  st = ups_db_insert(obj->db, txn_obj ? txn_obj->txn : 0,
                  &key, &record, (uint32_t)flags);
  if (st)
    RETURN_LONG(st);

  /* record number database? then store the key */
  if (obj->flags & UPS_RECORD_NUMBER32) {
    zk->type = IS_LONG;
    Z_LVAL_P(zk) = *(uint32_t *)key.data;
  }
  else if (obj->flags & UPS_RECORD_NUMBER64) {
    zk->type = IS_LONG;
    Z_LVAL_P(zk) = *(uint64_t *)key.data;
  }

  RETURN_LONG(0);
}
/* }}} */

/* {{{ proto string UpscaledbDatabase::find(mixed $txn, mixed $key,
 *                          integer flags = 0) */
PHP_METHOD(UpscaledbDatabase, find)
{
  zval *zk;
  zval *ztxn = 0;
  long flags = 0;
  upscaledb_db_object *obj;
  upscaledb_txn_object *txn_obj = 0;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O!z|l",
                          &ztxn, php_upscaledb_txn_class_entry, &zk, &flags)
                  == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  obj = (upscaledb_db_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  if (ztxn)
    txn_obj = (upscaledb_txn_object *)zend_object_store_get_object(ztxn TSRMLS_CC);

  ups_key_t key = {0};
  ups_record_t record = {0};

  ups_status_t st;
  uint8_t key_arena[16];
  st = zval_to_key(obj->key_type, zk, &key, key_arena);
  if (st)
    RETURN_LONG(st);

  st = ups_db_find(obj->db, txn_obj ? txn_obj->txn : 0,
                  &key, &record, (uint32_t)flags);
  if (st)
    RETURN_LONG(st);

  /* approx. matching enabled? then also copy the key */
  if (flags & UPS_FIND_NEAR_MATCH)
    item_to_zval(obj->key_type, key.data, key.size, zk);

  item_to_zval(obj->record_type, record.data, record.size, return_value);
}
/* }}} */

/* {{{ proto string UpscaledbDatabase::erase(mixed $txn, mixed $key,
 *                          integer flags = 0) */
PHP_METHOD(UpscaledbDatabase, erase)
{
  zval *zk;
  zval *ztxn = 0;
  long flags = 0;
  upscaledb_db_object *obj;
  upscaledb_txn_object *txn_obj;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O!z|l",
                          &ztxn, php_upscaledb_txn_class_entry, &zk, &flags)
                  == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  obj = (upscaledb_db_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  if (ztxn)
    txn_obj = (upscaledb_txn_object *)zend_object_store_get_object(ztxn TSRMLS_CC);

  ups_key_t key = {0};

  ups_status_t st;
  uint8_t key_arena[16];
  st = zval_to_key(obj->key_type, zk, &key, key_arena);
  if (st)
    RETURN_LONG(st);

  st = ups_db_erase(obj->db, txn_obj ? txn_obj->txn : 0, &key, (uint32_t)flags);
  RETURN_LONG(st);
}
/* }}} */

/* {{{ proto integer UpscaledbDatabase::close(integer $flags = 0) */
PHP_METHOD(UpscaledbDatabase, close)
{
  long flags = 0;
  upscaledb_db_object *obj;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &flags)
                  == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  obj = (upscaledb_db_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  if (obj->db) {
    ups_status_t st = ups_db_close(obj->db, (uint32_t)flags);
    if (st)
      RETURN_LONG(st);
    obj->db = 0;
  }

  if (obj->zenv) {
    zval_ptr_dtor(&obj->zenv);
    obj->zenv = 0;
  }

  RETURN_LONG(0);
}
/* }}} */

/* {{{ proto integer UpscaledbCursor::__construct($db, $txn = none,
 *              integer flags = 0) */
PHP_METHOD(UpscaledbCursor, __construct)
{
  zval *zdb;
  zval *ztxn = 0;
  upscaledb_cursor_object *obj;
  upscaledb_db_object *db_obj;
  upscaledb_txn_object *txn_obj = 0;
  long flags = 0;
  ups_status_t st;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|Ol", &zdb,
                          php_upscaledb_db_class_entry, &ztxn,
                          php_upscaledb_txn_class_entry, &flags)
                  == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  obj = (upscaledb_cursor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  db_obj = (upscaledb_db_object *)zend_object_store_get_object(zdb TSRMLS_CC);
  if (ztxn)
    txn_obj = (upscaledb_txn_object *)zend_object_store_get_object(ztxn TSRMLS_CC);

  st = ups_cursor_create(&obj->cursor, db_obj->db, txn_obj ? txn_obj->txn : 0,
                  (uint32_t)flags);
  obj->db = db_obj;
  obj->zdb = zdb;
  Z_ADDREF_P(zdb);
  RETURN_LONG(st);
}
/* }}} */

/* {{{ proto UpscaledbCursor UpscaledbCursor::clone() */
PHP_METHOD(UpscaledbCursor, clone)
{
  upscaledb_cursor_object *obj;
  upscaledb_cursor_object *c_obj;
  ups_status_t st = 0;

  if (zend_parse_parameters_none() == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  obj = (upscaledb_cursor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

  ups_cursor_t *cursor;
  st = ups_cursor_clone(obj->cursor, &cursor);
  if (st)
    RETURN_LONG(st);

  object_init_ex(return_value, php_upscaledb_cursor_class_entry);
  c_obj = (upscaledb_cursor_object *)zend_object_store_get_object(return_value TSRMLS_CC);
  c_obj->cursor = cursor;
  c_obj->db = obj->db;
  c_obj->zdb = obj->zdb;
  Z_ADDREF_P(c_obj->zdb);
}
/* }}} */

/* {{{ proto integer UpscaledbCursor::insert(mixed $key, mixed $value,
            integer $flags = 0) */
PHP_METHOD(UpscaledbCursor, insert)
{
  zval *zk = 0;
  zval *zr = 0;
  long flags = 0;
  upscaledb_cursor_object *obj;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|l", &zk,
                          &zr, &flags)
                  == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  obj = (upscaledb_cursor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

  ups_status_t st;
  uint8_t key_arena[16];
  ups_key_t key = {0};
  if ((obj->db->flags & (UPS_RECORD_NUMBER32 | UPS_RECORD_NUMBER64)) == 0) {
    st = zval_to_key(obj->db->key_type, zk, &key, key_arena);
    if (st)
      RETURN_LONG(st);
  }
  else if (zk->type != IS_NULL && Z_LVAL_P(zk) != 0)
    RETURN_LONG(UPS_INV_PARAMETER);

  uint8_t record_arena[16];
  ups_record_t record = {0};
  st = zval_to_record(obj->db->record_type, zr, &record, record_arena);
  if (st)
    RETURN_LONG(st);

  st = ups_cursor_insert(obj->cursor, &key, &record, (uint32_t)flags);
  if (st)
    RETURN_LONG(st);

  /* record number database? then store the key */
  if (obj->db->flags & UPS_RECORD_NUMBER32) {
    zk->type = IS_LONG;
    Z_LVAL_P(zk) = *(uint32_t *)key.data;
  }
  else if (obj->db->flags & UPS_RECORD_NUMBER64) {
    zk->type = IS_LONG;
    Z_LVAL_P(zk) = *(uint64_t *)key.data;
  }

  RETURN_LONG(0);
}
/* }}} */

/* {{{ proto integer UpscaledbCursor::overwrite(mixed $value,
            integer $flags = 0) */
PHP_METHOD(UpscaledbCursor, overwrite)
{
  zval *zr = 0;
  long flags = 0;
  upscaledb_cursor_object *obj;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|l", &zr, &flags)
                  == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  obj = (upscaledb_cursor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

  ups_status_t st;
  uint8_t record_arena[16];
  ups_record_t record = {0};
  st = zval_to_record(obj->db->record_type, zr, &record, record_arena);
  if (st)
    RETURN_LONG(st);

  st = ups_cursor_overwrite(obj->cursor, &record, (uint32_t)flags);
  RETURN_LONG(st);
}
/* }}} */

/* {{{ proto mixed UpscaledbCursor::find(mixed $key, integer $flags = 0) */
PHP_METHOD(UpscaledbCursor, find)
{
  zval *zk;
  long flags = 0;
  upscaledb_cursor_object *obj;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|l", &zk, &flags)
                  == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  obj = (upscaledb_cursor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

  ups_key_t key = {0};
  ups_record_t record = {0};

  ups_status_t st;
  uint8_t key_arena[16];
  st = zval_to_key(obj->db->key_type, zk, &key, key_arena);
  if (st)
    RETURN_LONG(st);

  st = ups_cursor_find(obj->cursor, &key, &record, (uint32_t)flags);
  if (st)
    RETURN_LONG(st);

  /* approx. matching enabled? then also copy the key */
  if (flags & UPS_FIND_NEAR_MATCH)
    item_to_zval(obj->db->key_type, key.data, key.size, zk);

  item_to_zval(obj->db->record_type, record.data, record.size, return_value);
}
/* }}} */

/* {{{ proto integer UpscaledbCursor::move(mixed &$key, mixed &$value,
                        integer $flags = 0) */
PHP_METHOD(UpscaledbCursor, move)
{
  zval *zk;
  zval *zr;
  long flags = 0;
  upscaledb_cursor_object *obj;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|l", &zk, &zr, &flags)
                  == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  obj = (upscaledb_cursor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

  ups_key_t key = {0};
  ups_record_t record = {0};

  ups_status_t st = ups_cursor_move(obj->cursor, zk->type == IS_NULL ? 0 : &key,
                  zk->type == IS_NULL ? 0 : &record, (uint32_t)flags);
  if (st)
    RETURN_LONG(st);

  if (zk->type != IS_NULL)
    item_to_zval(obj->db->key_type, key.data, key.size, zk);
  if (zr->type != IS_NULL)
    item_to_zval(obj->db->record_type, record.data, record.size, zr);

  RETURN_LONG(0);
}
/* }}} */

/* {{{ proto integer UpscaledbCursor::erase() */
PHP_METHOD(UpscaledbCursor, erase)
{
  upscaledb_cursor_object *obj;
  ups_status_t st = 0;

  if (zend_parse_parameters_none() == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  obj = (upscaledb_cursor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

  st = ups_cursor_erase(obj->cursor, 0);
  RETURN_LONG(st);
}
/* }}} */

/* {{{ proto integer UpscaledbCursor::get_duplicate_count() */
PHP_METHOD(UpscaledbCursor, get_duplicate_count)
{
  upscaledb_cursor_object *obj;
  ups_status_t st = 0;

  if (zend_parse_parameters_none() == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  obj = (upscaledb_cursor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

  uint32_t count;
  st = ups_cursor_get_duplicate_count(obj->cursor, &count, 0);
  if (st)
    RETURN_LONG(st);
  RETURN_LONG(count);
}
/* }}} */

/* {{{ proto integer UpscaledbCursor::get_record_size() */
PHP_METHOD(UpscaledbCursor, get_record_size)
{
  upscaledb_cursor_object *obj;
  ups_status_t st = 0;

  if (zend_parse_parameters_none() == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  obj = (upscaledb_cursor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

  uint32_t size;
  st = ups_cursor_get_record_size(obj->cursor, &size);
  if (st)
    RETURN_LONG(st);
  RETURN_LONG(size);
}
/* }}} */

/* {{{ proto integer UpscaledbCursor::close() */
PHP_METHOD(UpscaledbCursor, close)
{
  upscaledb_cursor_object *obj;
  ups_status_t st = 0;

  if (zend_parse_parameters_none() == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  obj = (upscaledb_cursor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

  if (obj->cursor != 0) {
    st = ups_cursor_close(obj->cursor);
    if (st)
      RETURN_LONG(st);
    obj->cursor = 0;
  }

  if (obj->zdb) {
    zval_ptr_dtor(&obj->zdb);
    obj->zdb = 0;
  }

  RETURN_LONG(0);
}
/* }}} */

/* {{{ proto UpscaledbTransaction::__construct($env, string $name = null,
                    integer $flags = 0) */
PHP_METHOD(UpscaledbTransaction, __construct)
{
  zval *zenv;
  char *name = 0;
  int name_length = 0;
  long flags = 0;
  upscaledb_txn_object *obj;
  upscaledb_object *env_obj;
  ups_status_t st;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|s!l", &zenv,
                          php_upscaledb_class_entry, &name, &name_length,
                          &flags)
                  == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  obj = (upscaledb_txn_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  env_obj = (upscaledb_object *)zend_object_store_get_object(zenv TSRMLS_CC);

  st = ups_txn_begin(&obj->txn, env_obj->env, name_length ? name : 0, 0, flags);
  if (st)
    RETURN_LONG(st);

  obj->zenv = zenv;
  Z_ADDREF_P(obj->zenv);
  RETURN_LONG(0);
}
/* }}} */

/* {{{ proto integer UpscaledbTransaction::abort() */
PHP_METHOD(UpscaledbTransaction, abort)
{
  upscaledb_txn_object *obj;

  if (zend_parse_parameters_none() == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  obj = (upscaledb_txn_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

  ups_status_t st = ups_txn_abort(obj->txn, 0);
  if (st)
    RETURN_LONG(st);

  if (obj->zenv) {
    zval_ptr_dtor(&obj->zenv);
    obj->zenv = 0;
  }

  RETURN_LONG(0);
}
/* }}} */

/* {{{ proto integer UpscaledbTransaction::commit() */
PHP_METHOD(UpscaledbTransaction, commit)
{
  upscaledb_txn_object *obj;

  if (zend_parse_parameters_none() == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  obj = (upscaledb_txn_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

  ups_status_t st = ups_txn_commit(obj->txn, 0);
  if (st)
    RETURN_LONG(st);

  if (obj->zenv) {
    zval_ptr_dtor(&obj->zenv);
    obj->zenv = 0;
  }

  RETURN_LONG(0);
}
/* }}} */

/* {{{ proto integer UpscaledbResult::__construct() */
PHP_METHOD(UpscaledbResult, __construct)
{
}
/* }}} */

/* {{{ proto integer UpscaledbResult::get_row_count() */
PHP_METHOD(UpscaledbResult, get_row_count)
{
  if (zend_parse_parameters_none() == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  upscaledb_result_object *ro;
  ro = (upscaledb_result_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_LONG(ro->row_count);
}
/* }}} */

/* {{{ proto integer UpscaledbResult::get_key_type() */
PHP_METHOD(UpscaledbResult, get_key_type)
{
  if (zend_parse_parameters_none() == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  upscaledb_result_object *ro;
  ro = (upscaledb_result_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_LONG(ro->key_type);
}
/* }}} */

/* {{{ proto integer UpscaledbResult::get_record_type() */
PHP_METHOD(UpscaledbResult, get_record_type)
{
  if (zend_parse_parameters_none() == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  upscaledb_result_object *ro;
  ro = (upscaledb_result_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_LONG(ro->record_type);
}
/* }}} */

/* {{{ proto integer UpscaledbResult::get_key() */
PHP_METHOD(UpscaledbResult, get_key)
{
  long index = 0;
  upscaledb_result_object *obj;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index)
                  == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  obj = (upscaledb_result_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

  if (index >= obj->row_count)
    RETURN_NULL();

  ups_key_t key = {0};
  uqi_result_get_key(obj->result, index, &key);
  item_to_zval(obj->key_type, key.data, key.size, return_value);
}
/* }}} */

/* {{{ proto integer UpscaledbResult::get_record() */
PHP_METHOD(UpscaledbResult, get_record)
{
  long index = 0;
  upscaledb_result_object *obj;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index)
                  == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  obj = (upscaledb_result_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

  if (index >= obj->row_count)
    RETURN_NULL();

  ups_record_t rec = {0};
  uqi_result_get_record(obj->result, index, &rec);
  item_to_zval(obj->record_type, rec.data, rec.size, return_value);
}
/* }}} */

/* {{{ proto integer UpscaledbResult::close() */
PHP_METHOD(UpscaledbResult, close)
{
  if (zend_parse_parameters_none() == FAILURE)
    RETURN_LONG(UPS_INV_PARAMETER);

  upscaledb_result_object *ro;
  ro = (upscaledb_result_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  uqi_result_close(ro->result);
  ro->result = 0;
  RETURN_LONG(0);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(upscaledb)
{
  zend_class_entry ce;
  zend_class_entry *exception_ce = zend_exception_get_default(TSRMLS_C);

  memcpy(&upscaledb_default_handlers, zend_get_std_object_handlers(),
          sizeof(zend_object_handlers));
  memcpy(&upscaledb_object_handlers, zend_get_std_object_handlers(),
          sizeof(zend_object_handlers));

  /* Register Upscaledb Class */
  INIT_CLASS_ENTRY(ce, "Upscaledb", php_upscaledb_class_methods);
  ce.create_object = php_upscaledb_object_new;
  php_upscaledb_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

  /* Register UpscaledbDatabase Class */
  INIT_CLASS_ENTRY(ce, "UpscaledbDatabase", php_upscaledb_db_class_methods);
  ce.create_object = php_upscaledb_db_object_new;
  php_upscaledb_db_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

  /* Register UpscaledbCursor Class */
  INIT_CLASS_ENTRY(ce, "UpscaledbCursor", php_upscaledb_cursor_class_methods);
  ce.create_object = php_upscaledb_cursor_object_new;
  php_upscaledb_cursor_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

  /* Register UpscaledbTransaction Class */
  INIT_CLASS_ENTRY(ce, "UpscaledbTransaction", php_upscaledb_txn_class_methods);
  ce.create_object = php_upscaledb_txn_object_new;
  php_upscaledb_txn_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

  /* Register UpscaledbResult Class */
  INIT_CLASS_ENTRY(ce, "UpscaledbResult", php_upscaledb_result_class_methods);
  ce.create_object = php_upscaledb_result_object_new;
  php_upscaledb_result_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

  /* Register UpscaledbException class */
  INIT_CLASS_ENTRY(ce, "UpscaledbException", NULL);
  ce.create_object = exception_ce->create_object;
  php_upscaledb_ce_UpscaledbException = zend_register_internal_class_ex(&ce,
          exception_ce, NULL TSRMLS_CC);

  /* Register constants */
  REGISTER_LONG_CONSTANT("UPS_TYPE_BINARY", 0,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_TYPE_CUSTOM", 1,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_TYPE_UINT8", 3,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_TYPE_UINT16", 5,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_TYPE_UINT32", 7,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_TYPE_UINT64", 9,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_TYPE_REAL32", 11,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_TYPE_REAL64", 12,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_SUCCESS", 0,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_INV_RECORD_SIZE", -2,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_INV_KEY_SIZE", -3,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_INV_PAGE_SIZE", -4,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_OUT_OF_MEMORY", -6,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_INV_PARAMETER", -8,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_INV_FILE_HEADER", -9,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_INV_FILE_VERSION", -10,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_KEY_NOT_FOUND", -11,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_DUPLICATE_KEY", -12,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_INTEGRITY_VIOLATED", -13,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_INTERNAL_ERROR", -14,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_WRITE_PROTECTED", -15,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_BLOB_NOT_FOUND", -16,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_IO_ERROR", -18,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_NOT_IMPLEMENTED", -20,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_FILE_NOT_FOUND", -21,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_WOULD_BLOCK", -22,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_NOT_READY", -23,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_LIMITS_REACHED", -24,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_ALREADY_INITIALIZED", -27,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_NEED_RECOVERY", -28,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_CURSOR_STILL_OPEN", -29,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_FILTER_NOT_FOUND", -30,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_TXN_CONFLICT", -31,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_TXN_STILL_OPEN", -33,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_CURSOR_IS_NIL", -100,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_DATABASE_NOT_FOUND", -200,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_DATABASE_ALREADY_EXISTS", -201,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_DATABASE_ALREADY_OPEN", -202,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_ENVIRONMENT_ALREADY_OPEN", -203,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_LOG_INV_FILE_HEADER", -300,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_NETWORK_ERROR", -400,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_PLUGIN_NOT_FOUND", -500,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_PARSER_ERROR", -501,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_PLUGIN_ALREADY_EXISTS", -502,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_TXN_READ_ONLY", 1,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_ENABLE_FSYNC", 0x00000001,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_READ_ONLY", 0x00000004,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_IN_MEMORY", 0x00000080,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_DISABLE_MMAP", 0x00000200,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_RECORD_NUMBER32", 0x00001000,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_RECORD_NUMBER64", 0x00002000,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_ENABLE_DUPLICATE_KEYS", 0x00004000,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_AUTO_RECOVERY", 0x00010000,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_ENABLE_TRANSACTIONS", 0x00020000,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_CACHE_UNLIMITED", 0x00040000,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_DISABLE_RECOVERY", 0x00080000,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_ENABLE_CRC32", 0x02000000,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_FLUSH_TRANSACTIONS_IMMEDIATELY", 0x08000000,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_OVERWRITE", 0x0001,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_DUPLICATE", 0x0002,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_DUPLICATE_INSERT_BEFORE", 0x0004,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_DUPLICATE_INSERT_AFTER", 0x0008,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_DUPLICATE_INSERT_FIRST", 0x0010,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_DUPLICATE_INSERT_LAST", 0x0020,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_HINT_APPEND", 0x00080000,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_HINT_PREPEND", 0x00100000,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_PARAM_JOURNAL_SWITCH_THRESHOLD", 0x00001,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_PARAM_CACHE_SIZE", 0x00000100,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_PARAM_PAGE_SIZE", 0x00000101,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_PARAM_KEY_SIZE", 0x00000102,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_PARAM_MAX_DATABASES", 0x00000103,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_PARAM_KEY_TYPE", 0x00000104,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_PARAM_LOG_DIRECTORY", 0x00000105,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_PARAM_ENCRYPTION_KEY", 0x00000106,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_PARAM_NETWORK_TIMEOUT_SEC", 0x00000107,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_PARAM_RECORD_SIZE", 0x00000108,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_PARAM_FILE_SIZE_LIMIT", 0x00000109,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_PARAM_POSIX_FADVISE", 0x00000110,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_PARAM_CUSTOM_COMPARE_NAME", 0x00000111,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_PARAM_RECORD_TYPE", 0x00000112,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_POSIX_FADVICE_NORMAL", 0,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_POSIX_FADVICE_RANDOM", 1,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_RECORD_SIZE_UNLIMITED", 4294967295,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_KEY_SIZE_UNLIMITED", 65535,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_PARAM_FLAGS", 0x00000200,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_PARAM_FILEMODE", 0x00000201,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_PARAM_FILENAME", 0x00000202,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_PARAM_DATABASE_NAME", 0x00000203,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_PARAM_MAX_KEYS_PER_PAGE", 0x00000204,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_PARAM_JOURNAL_COMPRESSION", 0x00001000,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_PARAM_RECORD_COMPRESSION", 0x00001001,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_PARAM_KEY_COMPRESSION", 0x00001002,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_COMPRESSOR_NONE", 0,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_COMPRESSOR_ZLIB", 1,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_COMPRESSOR_SNAPPY", 2,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_COMPRESSOR_LZF", 3,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_COMPRESSOR_UINT32_VARBYTE", 5,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_COMPRESSOR_UINT32_SIMDCOMP", 6,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_COMPRESSOR_UINT32_FOR", 10,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_COMPRESSOR_UINT32_SIMDFOR", 11,
                  CONST_CS | CONST_PERSISTENT);
  //REGISTER_LONG_CONSTANT("UPS_AUTO_CLEANUP", 1,
                  //CONST_CS | CONST_PERSISTENT); -- not allowed!
  REGISTER_LONG_CONSTANT("UPS_TXN_AUTO_ABORT", 4,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_TXN_AUTO_COMMIT", 8,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_CURSOR_FIRST", 0x0001,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_CURSOR_LAST", 0x0002,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_CURSOR_NEXT", 0x0004,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_CURSOR_PREVIOUS", 0x0008,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_SKIP_DUPLICATES", 0x0010,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_ONLY_DUPLICATES", 0x0020,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_FIND_LT_MATCH", 0x1000,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_FIND_GT_MATCH", 0x2000,
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_FIND_LEQ_MATCH", (0x1000 | 0x4000),
                  CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("UPS_FIND_GEQ_MATCH", (0x2000 | 0x4000),
                  CONST_CS | CONST_PERSISTENT);

  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(upscaledb)
{
  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(upscaledb)
{
  uint32_t maj, min, rev;
  ups_get_version(&maj, &min, &rev);
  char tmp[32];
  snprintf(tmp, sizeof(tmp), "%u.%u.%u", maj, min, rev);

  php_info_print_table_start();
  php_info_print_table_header(2, "upscaledb support", "enabled");
  php_info_print_table_row(2, "upscaledb extension version",
            PHP_UPSCALEDB_VERSION);
  php_info_print_table_row(2, "upscaledb library version", tmp);
  php_info_print_table_end();
}
/* }}} */

