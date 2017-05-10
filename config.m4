dnl $Id$
dnl config.m4 for extension rtrack

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(rtrack, for rtrack support,
dnl Make sure that the comment is aligned:
dnl [  --with-rtrack             Include rtrack support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(rtrack, whether to enable rtrack support,
dnl Make sure that the comment is aligned:
dnl [  --enable-rtrack           Enable rtrack support])

if test "$PHP_RTRACK" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-rtrack -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/rtrack.h"  # you most likely want to change this
  dnl if test -r $PHP_RTRACK/$SEARCH_FOR; then # path given as parameter
  dnl   RTRACK_DIR=$PHP_RTRACK
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for rtrack files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       RTRACK_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$RTRACK_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the rtrack distribution])
  dnl fi

  dnl # --with-rtrack -> add include path
  dnl PHP_ADD_INCLUDE($RTRACK_DIR/include)

  dnl # --with-rtrack -> check for lib and symbol presence
  dnl LIBNAME=rtrack # you may want to change this
  dnl LIBSYMBOL=rtrack # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $RTRACK_DIR/$PHP_LIBDIR, RTRACK_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_RTRACKLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong rtrack lib version or lib not found])
  dnl ],[
  dnl   -L$RTRACK_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(RTRACK_SHARED_LIBADD)

  PHP_NEW_EXTENSION(rtrack, rtrack.c, $ext_shared)
fi
