/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stephanie Wehner <_@r4k.net>                                |
   |          Fredrik Ohrn                                                |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"

#if HAVE_YP

#include "php_yp.h"

#include <rpcsvc/ypclnt.h>

/* {{{ thread safety stuff */

#ifdef ZTS
int yp_globals_id;
#else
PHP_YP_API php_yp_globals yp_globals;
#endif

/* }}} */

zend_function_entry yp_functions[] = {
	PHP_FE(yp_get_default_domain, NULL)
	PHP_FE(yp_order, NULL)
	PHP_FE(yp_master, NULL)
	PHP_FE(yp_match, NULL)
	PHP_FE(yp_first, NULL)
	PHP_FE(yp_next, NULL)
	PHP_FE(yp_all, NULL)
	PHP_FE(yp_cat, NULL)
	PHP_FE(yp_errno, NULL)
	PHP_FE(yp_err_string, NULL)
	{NULL, NULL, NULL}
};

zend_module_entry yp_module_entry = {
	STANDARD_MODULE_HEADER,
	"yp",
	yp_functions,
	PHP_MINIT(yp),
	NULL,
	PHP_RINIT(yp),
	NULL,
	PHP_MINFO(yp),
	PHP_YP_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_YP
ZEND_GET_MODULE(yp)
#endif

/* {{{ proto string yp_get_default_domain(void)
   Returns the domain or false */
PHP_FUNCTION(yp_get_default_domain)
{
	char *outdomain;

	if (ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}

	if((YP(error) = yp_get_default_domain(&outdomain))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", yperr_string (YP(error)));
		RETURN_FALSE;
	}
	RETVAL_STRING(outdomain,1);
}
/* }}} */

/* {{{ proto int yp_order(string domain, string map)
   Returns the order number or false */
PHP_FUNCTION(yp_order)
{
	char *domain = NULL, *map = NULL;
	int domain_len = 0, map_len = 0;

#if SOLARIS_YP
	unsigned long outval = 0;
#else
	int outval = 0;
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
				&domain, &domain_len, &map, &map_len) == FAILURE) {
        return;
	}

	if((YP(error) = yp_order(domain, map, &outval))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", yperr_string (YP(error)));
		RETURN_FALSE;
	}

	RETVAL_LONG(outval);
}
/* }}} */

/* {{{ proto string yp_master(string domain, string map)
   Returns the machine name of the master */
PHP_FUNCTION(yp_master)
{
	char *domain = NULL, *map = NULL, *outname = NULL;
	int domain_len = 0, map_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
				&domain, &domain_len, &map, &map_len) == FAILURE) {
        return;
	}

	if((YP(error) = yp_master(domain, map, &outname))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", yperr_string (YP(error)));
		RETURN_FALSE;
	}

	RETVAL_STRING(outname, 1);
}
/* }}} */

/* {{{ proto string yp_match(string domain, string map, string key)
   Returns the matched line or false */
PHP_FUNCTION(yp_match)
{
	char *domain = NULL, *map = NULL, *key = NULL, *outval = NULL;
	int domain_len = 0, map_len = 0, key_len = 0, outvallen = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &domain,
				&domain_len, &map, &map_len, &key, &key_len) == FAILURE) {
        return;
	}

	if((YP(error) = yp_match(domain, map, key, key_len, &outval, &outvallen))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", yperr_string (YP(error)));
		RETVAL_FALSE;
	} else {
		RETVAL_STRINGL(outval, outvallen, 1);
	}

	if (outval) {
		free(outval);
	}
}
/* }}} */

/* {{{ proto array yp_first(string domain, string map)
   Returns the first key as array with $var[$key] and the the line as the value */
PHP_FUNCTION(yp_first)
{
	char *domain = NULL, *map = NULL, *outkey = NULL, *outval = NULL;
	int domain_len = 0, map_len = 0, outkey_len, outval_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
				&domain, &domain_len, &map, &map_len) == FAILURE) {
        return;
	}

	if((YP(error) = yp_first(domain, map, &outkey, &outkey_len, &outval, &outval_len))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", yperr_string (YP(error)));
		RETVAL_FALSE;
	} else {
		array_init(return_value);
		add_assoc_stringl_ex(return_value, outkey, outkey_len + 1, outval, outval_len, 1);
	}

	if (outval) {
		free(outval);
	}
}
/* }}} */

/* {{{ proto array yp_next(string domain, string map, string key)
   Returns an array with $var[$key] and the the line as the value */
PHP_FUNCTION(yp_next)
{
	char *domain = NULL, *map = NULL, *key = NULL;
	int domain_len = 0, map_len = 0, key_len = 0;
	char *outkey = NULL, *outval = NULL;
	int outkey_len = 0, outval_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &domain,
				&domain_len, &map, &map_len, &key, &key_len) == FAILURE) {
        return;
	}

	if((YP(error) = yp_next(domain, map, key, key_len, &outkey, &outkey_len, &outval, &outval_len))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", yperr_string (YP(error)));
		RETVAL_FALSE;
	} else {
		array_init(return_value);
		add_assoc_stringl_ex(return_value, outkey, outkey_len + 1, outval, outval_len, 1);
    }

	if (outval) {
		free(outval);
	}
}
/* }}} */

/* {{{ php_foreach_all
 */
static int php_foreach_all (int instatus, char *inkey, int inkeylen, char *inval, int invallen, char *indata)
{
	int is_stop = 0;
	zval *args;
	zval *retval;
	TSRMLS_FETCH();

	MAKE_STD_ZVAL(args);
	array_init(args);
	add_index_long(args, 0, instatus);
	add_index_stringl(args, 1, inkey, inkeylen, 1);
	add_index_stringl(args, 2, inval, invallen, 1);

	php_yp_all_callback *cb = (php_yp_all_callback *) indata;
	zend_fcall_info_args(&cb->fci, args TSRMLS_CC);
	zend_fcall_info_call(&cb->fci, &cb->fcc, &retval, args TSRMLS_CC);
	zend_fcall_info_args_clear(&cb->fci, 1);

	if (retval) {
		is_stop = zval_is_true(retval);
		zval_ptr_dtor(&retval);
	}

	return is_stop;
}
/* }}} */

/* {{{ proto bool yp_all(string domain, string map, string callback)
   Traverse the map and call a function on each entry */
PHP_FUNCTION(yp_all)
{
	char *domain = NULL, *map = NULL;
	int domain_len = 0, map_len = 0;
	php_yp_all_callback *foreach_cb = emalloc(sizeof(php_yp_all_callback));

	struct ypall_callback callback;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssf",
				&domain, &domain_len, &map, &map_len,
				&foreach_cb->fci, &foreach_cb->fcc) == FAILURE) {
        return;
	}

	callback.foreach = php_foreach_all;
	callback.data = foreach_cb;

	yp_all(domain, map, &callback);

	efree(foreach_cb);
	RETURN_FALSE;
}
/* }}} */

/* {{{ php_foreach_cat
 */
static int php_foreach_cat (int instatus, char *inkey, int inkeylen, char *inval, int invallen, char *indata)
{
	int err;

	err = ypprot_err (instatus);

	if (!err)
	{
		if (inkeylen) {
			char *key = emalloc(inkeylen+1);
			strlcpy(key, inkey, inkeylen+1);
			add_assoc_stringl_ex((zval *) indata, key, inkeylen+1, inval, invallen, 1);
			efree(key);
		}

		return 0;
	}

	if (err != YPERR_NOMORE)
	{
		TSRMLS_FETCH();

		YP(error) = err;
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", yperr_string (err));
	}

	return 0;
}
/* }}} */

/* {{{ proto array yp_cat(string domain, string map)
   Return an array containing the entire map */
PHP_FUNCTION(yp_cat)
{
	char *domain = NULL, *map = NULL;
	int domain_len = 0, map_len = 0;
	struct ypall_callback callback;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
				&domain, &domain_len, &map, &map_len) == FAILURE) {
        return;
	}

	array_init(return_value);

	callback.foreach = php_foreach_cat;
	callback.data = (char *) return_value;

	yp_all(domain, map, &callback);
}
/* }}} */

/* {{{ proto int yp_errno()
   Returns the error code from the last call or 0 if no error occured */
PHP_FUNCTION(yp_errno)
{
	if((ZEND_NUM_ARGS() != 0)) {
		WRONG_PARAM_COUNT;
	}

	RETURN_LONG (YP(error));
}
/* }}} */

/* {{{ proto string yp_err_string(int errorcode)
   Returns the corresponding error string for the given error code */
PHP_FUNCTION(yp_err_string)
{
	int errcode = 0;
	char *string = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &errcode) == FAILURE) {
        return;
	}

	if((string = yperr_string(errcode)) == NULL) {
		RETURN_FALSE;
	}

	RETVAL_STRING(string, 1);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(yp)
{
#ifdef ZTS
	ts_allocate_id(&yp_globals_id, sizeof(php_yp_globals), NULL, NULL);
#endif

	REGISTER_LONG_CONSTANT("YPERR_ACCESS", YPERR_ACCESS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_BADARGS", YPERR_BADARGS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_BADDB", YPERR_BADDB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_BUSY", YPERR_BUSY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_DOMAIN", YPERR_DOMAIN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_KEY", YPERR_KEY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_MAP", YPERR_MAP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_NODOM", YPERR_NODOM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_NOMORE", YPERR_NOMORE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_PMAP", YPERR_PMAP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_RESRC", YPERR_RESRC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_RPC", YPERR_RPC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_YPBIND", YPERR_YPBIND, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_YPERR", YPERR_YPERR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_YPSERV", YPERR_YPSERV, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_VERS", YPERR_VERS, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

PHP_RINIT_FUNCTION(yp)
{
	YP(error) = 0;
	
	return SUCCESS;
}

PHP_MINFO_FUNCTION(yp)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "YP Support", "enabled");
	php_info_print_table_row(2, "Version", PHP_YP_VERSION);
	php_info_print_table_end();
}
#endif /* HAVE_YP */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
