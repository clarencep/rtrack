/*
 * php_screw
 * (C) 2007, Kunimasa Noda/PM9.com, Inc. <http://www.pm9.com,  kuni@pm9.com>
 * see file LICENSE for license details
 */

#include "php.h"
#include "php_ini.h"
#include "ext/standard/file.h"
#include "ext/standard/info.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include "rtrack.h"


PHP_MINIT_FUNCTION(rtrack);
PHP_MSHUTDOWN_FUNCTION(rtrack);
PHP_MINFO_FUNCTION(rtrack);

ZEND_API zend_op_array *(*org_compile_file)(zend_file_handle *file_handle, int type TSRMLS_DC);

ZEND_API void rtrack_record_log(const char* filename)
{
  time_t current_time = {0};
  struct tm * time_info = {0};
  char timeString[32] = {0};  // space for "YYYY-mm-dd HH:MM:SS \0"


  FILE *fp = fopen("require.log", "a+");
  if (!fp){
    return;
  }

  time(&current_time);
  time_info = localtime(&current_time);

  strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S ", time_info);

  fputs(timeString, fp);
  fputs(filename, fp);
  fputs("\n", fp);

  fflush(fp);
  fclose(fp);


  // zval file_zval;
  // INIT_ZVAL(file_zval);
  // ZVAL_STRING(&file_zval, "/tmp/require.log", 1);

  // zval msg_zval;
  // INIT_ZVAL(msg_zval);
  // ZVAL_STRING(&msg_zval, filename, 1);


  // zval flag_zval;
  // INIT_ZVAL(flag_zval);
  // ZVAL_LONG(&flag_zval, 8);

  // zval *params[] = { &file_zval, &msg_zval, &flag_zval };
  // zend_uint param_count = 3;
  // zval *retval_ptr;

  // zval function_name;
  // INIT_ZVAL(function_name);
  // ZVAL_STRING(&function_name, "file_put_contents", 1);

  // if (call_user_function(
  //         CG(function_table), NULL /* no object */, &function_name,
  //         retval_ptr, param_count, params TSRMLS_CC
  //     ) == SUCCESS
  // ) {
  //     /* do something with retval_ptr here if you like */
  // }

  // /* don't forget to free the zvals */
  // zval_ptr_dtor(&retval_ptr);
  // zval_dtor(&function_name);
  // zval_dtor(&file_zval);
  // zval_dtor(&msg_zval);
  // zval_dtor(&flag_zval);
}

ZEND_API zend_op_array *rtrack_compile_file(zend_file_handle *file_handle, int type TSRMLS_DC)
{
  rtrack_record_log(file_handle->filename);
  return org_compile_file(file_handle, type);
}

zend_module_entry rtrack_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
  STANDARD_MODULE_HEADER,
#endif
  "rtrack",
  NULL,
  PHP_MINIT(rtrack),
  PHP_MSHUTDOWN(rtrack),
  NULL,
  NULL,
  PHP_MINFO(rtrack),
#if ZEND_MODULE_API_NO >= 20010901
  "1.5.0", /* Replace with version number for your extension */
#endif
  STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(rtrack);

PHP_MINFO_FUNCTION(rtrack)
{
  php_info_print_table_start();
  php_info_print_table_header(2, "rtrack support", "enabled");
  php_info_print_table_end();
}

PHP_MINIT_FUNCTION(rtrack)
{
  CG(compiler_options) |= ZEND_COMPILE_EXTENDED_INFO;
  org_compile_file = zend_compile_file;
  zend_compile_file = rtrack_compile_file;
  return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(rtrack)
{
  CG(compiler_options) |= ZEND_COMPILE_EXTENDED_INFO;
  zend_compile_file = org_compile_file;
  return SUCCESS;
}
