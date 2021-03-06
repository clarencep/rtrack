/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_rtrack.h"

#ifndef PHP_FE_END
	#define PHP_FE_END {NULL, NULL, NULL}
#endif

/* If you declare any globals in php_rtrack.h uncomment this:
*/
ZEND_DECLARE_MODULE_GLOBALS(rtrack)

/* True global resources - no need for thread safety here */
static int le_rtrack;

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
    // STD_PHP_INI_ENTRY("rtrack.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_rtrack_globals, rtrack_globals)
    STD_PHP_INI_ENTRY("rtrack.log_file", "", PHP_INI_ALL, OnUpdateString, log_file, zend_rtrack_globals, rtrack_globals)
PHP_INI_END()
/* }}} */

ZEND_API zend_op_array *(*org_compile_file)(zend_file_handle *file_handle, int type TSRMLS_DC);

static void rtrack_call_hook_func(const char* hook_func, const char* filename)
{
	zval	retval;
	zval	*zfuncname;
	zval    *param;
	char	*message;

	INIT_ZVAL(retval);
	MAKE_STD_ZVAL(zfuncname);
	MAKE_STD_ZVAL(param);

	ZVAL_STRING(zfuncname, hook_func, 1);
	ZVAL_STRING(param, filename, 1);

	if (call_user_function(EG(function_table), NULL, zfuncname, &retval, 1, &param TSRMLS_CC) != SUCCESS) {

		spprintf(&message, 0, "Error calling hook_func: %s", hook_func);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, message);
		efree(message);

		zval_ptr_dtor(&zfuncname);
		zval_ptr_dtor(&param);
		return;
	}

	zval_ptr_dtor(&zfuncname);
	zval_ptr_dtor(&param);
}

static void rtrack_write_log(const char* filename)
{
	time_t current_time = {0};
	struct tm * time_info = {0};
	char timeString[32] = {0};  // space for "YYYY-mm-dd HH:MM:SS \0"
	FILE *fp;
	const char *log_file;

	log_file = RTRACK_G(log_file);
	if (!log_file || !*log_file){
		return;
	}

	fp = fopen(log_file, "a+");
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
}

static void rtrack_record_log(const char* filename)
{
	const char *hook_func = RTRACK_G(hook_func);
	if (hook_func && *hook_func){
		rtrack_call_hook_func(hook_func, filename);
		return;
	}
	
	rtrack_write_log(filename);
}

zend_op_array *rtrack_compile_file(zend_file_handle *file_handle, int type TSRMLS_DC)
{
	RTRACK_G(count)++;
	rtrack_record_log(file_handle->filename);
	return org_compile_file(file_handle, type);
}


/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_rtrack_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_rtrack_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "rtrack", arg);
	RETURN_STRINGL(strg, len, 0);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/

static int _rtrack_get_hook_func(char **p_hook_func){
	char *hook_func;

	if (p_hook_func == NULL) {
		return 0;
	}

	hook_func = RTRACK_G(hook_func);
	if (!hook_func){
		*p_hook_func = "";
		return 0;
	}

	*p_hook_func = hook_func;
	return strlen(hook_func);
}

/* {{{ proto string rtrack_get_hook_func()
   Get the hook func string. */
PHP_FUNCTION(rtrack_get_hook_func)
{
	int len;
	char *hook_func;

	len = _rtrack_get_hook_func(&hook_func);

	RETURN_STRINGL(hook_func, len, 1);
}
/* }}} */

/* {{{ proto string rtrack_set_hook_func(string func)
   Set the hook func string. Return the previous hook func string */
PHP_FUNCTION(rtrack_set_hook_func)
{
	char *arg = NULL;
	int arg_len, len;
	char *prev_hook_func;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = _rtrack_get_hook_func(&prev_hook_func);
	
	if (RTRACK_G(hook_func)){
		efree(RTRACK_G(hook_func));
	}

	RTRACK_G(hook_func) = strdup(arg);

	RETURN_STRINGL(prev_hook_func, len, 1);
}
/* }}} */

/* {{{ proto int rtrack_count()
   Return a int of how many files are required/included */
PHP_FUNCTION(rtrack_count)
{
	long n = RTRACK_G(count);
	RETURN_LONG(n);
}
/* }}} */

/* {{{ php_rtrack_init_globals
 */
static void php_rtrack_init_globals(zend_rtrack_globals *rtrack_globals)
{
	rtrack_globals->count = 0;
	rtrack_globals->log_file = NULL;
	rtrack_globals->hook_func = NULL;
}
/* }}} */

static void php_rtrack_free_hook_func_if_needed() 
{
	if (rtrack_globals.hook_func){
		efree(rtrack_globals.hook_func);
		rtrack_globals.hook_func = NULL;
	}
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(rtrack)
{
	REGISTER_INI_ENTRIES();
	
	php_rtrack_init_globals(&rtrack_globals);
	
	CG(compiler_options) |= ZEND_COMPILE_EXTENDED_INFO;
	org_compile_file = zend_compile_file;
	zend_compile_file = rtrack_compile_file;

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(rtrack)
{
	UNREGISTER_INI_ENTRIES();

	php_rtrack_init_globals(&rtrack_globals);
	php_rtrack_free_hook_func_if_needed();


	CG(compiler_options) |= ZEND_COMPILE_EXTENDED_INFO;
	zend_compile_file = org_compile_file;

	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(rtrack)
{
	php_rtrack_init_globals(&rtrack_globals);

	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(rtrack)
{
	php_rtrack_init_globals(&rtrack_globals);
	php_rtrack_free_hook_func_if_needed();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(rtrack)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "rtrack support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ rtrack_functions[]
 *
 * Every user visible function must have an entry in rtrack_functions[].
 */
const zend_function_entry rtrack_functions[] = {
	PHP_FE(confirm_rtrack_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(rtrack_count,	NULL)
	PHP_FE(rtrack_get_hook_func,	NULL)
	PHP_FE(rtrack_set_hook_func,	NULL)
	PHP_FE_END	/* Must be the last line in rtrack_functions[] */
};
/* }}} */

/* {{{ rtrack_module_entry
 */
zend_module_entry rtrack_module_entry = {
	STANDARD_MODULE_HEADER,
	"rtrack",
	rtrack_functions,
	PHP_MINIT(rtrack),
	PHP_MSHUTDOWN(rtrack),
	PHP_RINIT(rtrack),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(rtrack),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(rtrack),
	PHP_RTRACK_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_RTRACK
ZEND_GET_MODULE(rtrack)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
