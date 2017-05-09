dnl config.m4 for extension rtrack
dnl don't forget to call PHP_EXTENSION(rtrack)
dnl If your extension references something external, use with:

PHP_ARG_WITH(rtrack, for rtrack support,
dnl Make sure that the comment is aligned:
[  --with-rtrack             Include rtrack support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(rtrack, whether to enable rtrack support,
dnl Make sure that the comment is aligned:
[  --enable-rtrack           Enable rtrack support])

if test "$PHP_php_screw" != "no"; then
  dnl Action..
  PHP_EXTENSION(rtrack, $ext_shared)
fi
