dnl $Id$
dnl config.m4 for extension upscaledb

PHP_ARG_WITH(upscaledb, for upscaledb support,
[  --with-upscaledb[=Path]             Include upscaledb support])

if test "$php_upscaledb" != "no"; then

  # --with-upscaledb -> check with-path
  SEARCH_PATH="/usr/local /usr /home/ruppc/prj/upscaledb/src/.libs /home/ruppc/prj/upscaledb /home/chris/prj/upscaledb/src/.libs /home/chris/prj/upscaledb"
  SEARCH_FOR="include/ups/upscaledb.h"
  SEARCH_LIB="libupscaledb"

  dnl search upscaledb
  AC_MSG_CHECKING([for upscaledb location])
  for i in $PHP_UPSCALEDB $SEARCH_PATH ; do
    if test -r $i/$SEARCH_FOR; then
	  UPSCALEDB_INCLUDE_DIR=$i
	  AC_MSG_RESULT(upscaledb headers found in $i)
    fi

    if test -r $i/$PHP_LIBDIR/$SEARCH_LIB.a || test -r $i/$PHP_LIBDIR/$SEARCH_LIB.$SHLIB_SUFFIX_NAME; then
	  UPSCALEDB_LIB_DIR=$i/$PHP_LIBDIR
	  AC_MSG_RESULT(upscaledb lib found in $i/lib)
    fi

	dnl from upscaledb build dir
    if test -r $i/$SEARCH_LIB.a || test -r $i/$SEARCH_LIB.$SHLIB_SUFFIX_NAME; then
	  UPSCALEDB_LIB_DIR=$i
	  AC_MSG_RESULT(upscaledb lib found in $i)
    fi

    if test -z "$UPSCALEDB_LIB_DIR"; then
      for j in "lib/x86_64-linux-gnu" "lib/x86_64-linux-gnu"; do
        echo find "--$i/$j"
        if test -r $i/$j/$SEARCH_LIB.a || test -r $i/$j/$SEARCH_LIB.$SHLIB_SUFFIX_NAME; then
          UPSCALEDB_LIB_DIR=$i/$j
          AC_MSG_RESULT(upscaledb lib found in $i/$j)
        fi
      done
    fi
  done
  
  if test -z "$UPSCALEDB_INCLUDE_DIR" || test -z "$UPSCALEDB_LIB_DIR"; then
    AC_MSG_RESULT([upscaledb not found])
    AC_MSG_ERROR([Please reinstall the upscaledb library])
  fi

  # --with-upscaledb -> add include path
  PHP_ADD_INCLUDE($UPSCALEDB_INCLUDE_DIR/include)

  # --with-upscaledb -> check for lib and symbol presence
  LIBNAME=upscaledb
  PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $UPSCALEDB_LIB_DIR, UPSCALEDB_SHARED_LIBADD)
  
  PHP_SUBST(UPSCALEDB_SHARED_LIBADD)

  PHP_NEW_EXTENSION(upscaledb, upscaledb.c, $ext_shared)
fi
