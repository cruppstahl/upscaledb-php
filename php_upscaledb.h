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

#ifndef PHP_UPSCALEDB_H
#define PHP_UPSCALEDB_H

#define PHP_UPSCALEDB_VERSION "0.0.1"

extern zend_module_entry upscaledb_module_entry;
#define phpext_upscaledb_ptr &upscaledb_module_entry

#ifdef PHP_WIN32
#	define PHP_UPSCALEDB_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_UPSCALEDB_API __attribute__ ((visibility("default")))
#else
#	define PHP_UPSCALEDB_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_METHOD(Upscaledb, __construct);
PHP_METHOD(Upscaledb, create);
PHP_METHOD(Upscaledb, open);
PHP_METHOD(Upscaledb, create_db);
PHP_METHOD(Upscaledb, open_db);
PHP_METHOD(Upscaledb, erase_db);
PHP_METHOD(Upscaledb, rename_db);
PHP_METHOD(Upscaledb, flush);
PHP_METHOD(Upscaledb, select_range);
PHP_METHOD(Upscaledb, get_database_names);
PHP_METHOD(Upscaledb, close);
PHP_METHOD(Upscaledb, strerror);

PHP_METHOD(UpscaledbDatabase, __construct);
PHP_METHOD(UpscaledbDatabase, insert);
PHP_METHOD(UpscaledbDatabase, find);
PHP_METHOD(UpscaledbDatabase, erase);
PHP_METHOD(UpscaledbDatabase, close);

PHP_METHOD(UpscaledbCursor, __construct);
PHP_METHOD(UpscaledbCursor, insert);
PHP_METHOD(UpscaledbCursor, find);
PHP_METHOD(UpscaledbCursor, erase);
PHP_METHOD(UpscaledbCursor, move);
PHP_METHOD(UpscaledbCursor, clone);
PHP_METHOD(UpscaledbCursor, overwrite);
PHP_METHOD(UpscaledbCursor, get_duplicate_count);
PHP_METHOD(UpscaledbCursor, get_record_size);
PHP_METHOD(UpscaledbCursor, close);

PHP_METHOD(UpscaledbTransaction, __construct);
PHP_METHOD(UpscaledbTransaction, commit);
PHP_METHOD(UpscaledbTransaction, abort);

PHP_METHOD(UpscaledbResult, __construct);
PHP_METHOD(UpscaledbResult, get_row_count);
PHP_METHOD(UpscaledbResult, get_key_type);
PHP_METHOD(UpscaledbResult, get_record_type);
PHP_METHOD(UpscaledbResult, get_key);
PHP_METHOD(UpscaledbResult, get_record);
PHP_METHOD(UpscaledbResult, close);

PHP_MINIT_FUNCTION(upscaledb);
PHP_MSHUTDOWN_FUNCTION(upscaledb);
PHP_RINIT_FUNCTION(upscaledb);
PHP_RSHUTDOWN_FUNCTION(upscaledb);
PHP_MINFO_FUNCTION(upscaledb);

#endif	/* PHP_UPSCALEDB_H */

