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
#include "zend_exceptions.h"

#if HAVE_YP

#include "php_yp.h"
#include "ext/spl/spl_exceptions.h"

#include <rpcsvc/ypclnt.h>

#ifndef PHP_FE_END
#define PHP_FE_END	{NULL, NULL, NULL, 0, 0}
#endif

/* {{{ thread safety stuff */

#ifdef ZTS
int yp_globals_id;
#else
PHP_YP_API php_yp_globals yp_globals;
#endif

/* }}} */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_yp_get_default_domain, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yp_order, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, domain)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yp_master, 0, ZEND_RETURN_VALUE, 2)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, mapname)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yp_match, 0, ZEND_RETURN_VALUE, 3)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, mapname)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yp_first, 0, ZEND_RETURN_VALUE, 2)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, mapname)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yp_next, 0, ZEND_RETURN_VALUE, 3)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, mapname)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yp_all, 0, ZEND_RETURN_VALUE, 3)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, mapname)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yp_cat, 0, ZEND_RETURN_VALUE, 2)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, mapname)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yp_errno, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yp_err_string, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, errorcode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yp_class_construct, 0, ZEND_RETURN_VALUE, 0)
	ZEND_ARG_INFO(0, domain)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yp_class_getDomain, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yp_class_setDomain, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, domain)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yp_class_cat, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, mapname)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yp_class_match, 0, ZEND_RETURN_VALUE, 2)
	ZEND_ARG_INFO(0, mapname)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yp_class_first, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, mapname)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yp_class_next, 0, ZEND_RETURN_VALUE, 2)
	ZEND_ARG_INFO(0, mapname)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()
/* }}} */

zend_function_entry yp_functions[] = {
	PHP_FE(yp_get_default_domain, arginfo_yp_get_default_domain)
	PHP_FE(yp_order, arginfo_yp_order)
	PHP_FE(yp_master, arginfo_yp_master)
	PHP_FE(yp_match, arginfo_yp_match)
	PHP_FE(yp_first, arginfo_yp_first)
	PHP_FE(yp_next, arginfo_yp_next)
	PHP_FE(yp_all, arginfo_yp_all)
	PHP_FE(yp_cat, arginfo_yp_cat)
	PHP_FE(yp_errno, arginfo_yp_errno)
	PHP_FE(yp_err_string, arginfo_yp_err_string)
	PHP_FE_END
};

zend_class_entry *yp_ce_YP;
zend_class_entry *yp_ce_YPException;
static int php_foreach_cat(int instatus, char *inkey, int inkeylen, char *inval, int invallen, char *indata);

/* {{{ proto void NIS\YP::__construct(string domain) */
PHP_METHOD(YP, __construct)
{
	zval *obj = NULL;
	char *domain = NULL;
	int domain_len = 0;
	int error = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!",
				&domain, &domain_len) == FAILURE) {
		return;
	}

	if (!domain) {
		error = yp_get_default_domain(&domain);
		if (error) {
			zend_throw_exception_ex(yp_ce_YPException, error TSRMLS_CC, yperr_string(error));
			return;
		}
		domain_len = strlen(domain);
	}

	obj = getThis();
	zend_update_property_stringl(yp_ce_YP, obj, "domain", sizeof("domain") - 1,
			domain, domain_len TSRMLS_DC);
}
/* }}} */

/* {{{ proto string NIS\YP::getDomain(void) */
PHP_METHOD(YP, getDomain)
{
	zval *obj = NULL;
	zval *prop = NULL;

	if (ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}

	obj = getThis();
	prop = zend_read_property(yp_ce_YP, obj, "domain", sizeof("domain") - 1, 1 TSRMLS_CC);
	RETURN_STRINGL(estrdup(Z_STRVAL_P(prop)), Z_STRLEN_P(prop), 0);
}
/* }}} */

/* {{{ proto void NIS\YP::getDomain(string domain) */
PHP_METHOD(YP, setDomain)
{
	zval *obj = NULL;
	char *domain = NULL;
	int domain_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
				&domain, &domain_len) == FAILURE) {
		return;
	}

	obj = getThis();
	zend_update_property_stringl(yp_ce_YP, obj, "domain", sizeof("domain") - 1,
			domain, domain_len TSRMLS_DC);
}
/* }}} */

/* {{{ proto array NIS\YP::cat(string map) */
PHP_METHOD(YP, cat)
{
	zval *obj = NULL;
	zval *prop = NULL;
	char *domain = NULL;
	char *mapname = NULL;
	int mapname_len = 0;
	struct ypall_callback callback;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
				&mapname, &mapname_len) == FAILURE) {
		return;
	}

	obj = getThis();

	array_init(return_value);

	callback.foreach = php_foreach_cat;
	callback.data = (char *) return_value;

	prop = zend_read_property(yp_ce_YP, obj, "domain", sizeof("domain") - 1, 0 TSRMLS_CC);
	domain = Z_STRVAL_P(prop);
	yp_all(domain, mapname, &callback);
}
/* }}} */

/* {{{ proto string NIS\YP::match(string mapname, string key) */
PHP_METHOD(YP, match)
{
    zval *obj = NULL, *prop = NULL;
	char *domain = NULL, *mapname = NULL, *key = NULL, *outval = NULL;
	int domain_len = 0, mapname_len = 0, key_len = 0, outvallen = 0, error = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
				&mapname, &mapname_len, &key, &key_len) == FAILURE) {
		return;
	}

	obj = getThis();

	prop = zend_read_property(yp_ce_YP, obj, "domain", sizeof("domain") - 1, 0 TSRMLS_CC);
	domain = Z_STRVAL_P(prop);

	error = yp_match(domain, mapname, key, key_len, &outval, &outvallen);
	if (error) {
		zend_throw_exception_ex(yp_ce_YPException, error TSRMLS_CC, yperr_string(error));
		return;
	}

	RETVAL_STRINGL(outval, outvallen, 1);
}
/* }}} */

/* {{{ proto array NIS\YP::first(string mapname) */
PHP_METHOD(YP, first)
{
	zval *obj = NULL, *prop = NULL;
	char *domain = NULL, *mapname = NULL, *outkey = NULL, *outval = NULL;
	int domain_len = 0, mapname_len = 0, outkey_len = 0, outval_len = 0, error = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
				&mapname, &mapname_len) == FAILURE) {
		return;
	}

	obj = getThis();
	prop = zend_read_property(yp_ce_YP, obj, "domain", sizeof("domain") - 1, 0 TSRMLS_CC);
	domain = Z_STRVAL_P(prop);

	error = yp_first(domain, mapname, &outkey, &outkey_len, &outval, &outval_len);
	if (error) {
		zend_throw_exception_ex(yp_ce_YPException, error TSRMLS_CC, yperr_string(error));
		return;
	}

	array_init(return_value);
	add_assoc_stringl_ex(return_value, outkey, outkey_len + 1, outval, outval_len, 1);
}
/* }}} */

/* {{{ proto array NIS\YP::next(string mapname, string key) */
PHP_METHOD(YP, next)
{
	zval *obj = NULL, *prop = NULL;
	char *domain = NULL, *mapname = NULL, *key = NULL;
	int domain_len = 0, mapname_len = 0, key_len = 0;
	char *outkey = NULL, *outval = NULL;
	int outkey_len = 0, outval_len = 0, error = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
				&mapname, &mapname_len, &key, &key_len) == FAILURE) {
		return;
	}

	obj = getThis();
	prop = zend_read_property(yp_ce_YP, obj, "domain", sizeof("domain") - 1, 0 TSRMLS_CC);
	domain = Z_STRVAL_P(prop);

	error = yp_next(domain, mapname, key, key_len, &outkey, &outkey_len, &outval, &outval_len);
	if (error) {
		zend_throw_exception_ex(yp_ce_YPException, error TSRMLS_CC, yperr_string(error));
		return;
	}

	array_init(return_value);
	add_assoc_stringl_ex(return_value, outkey, outkey_len + 1, outval, outval_len, 1);
}
/* }}} */

static const zend_function_entry yp_YP_methods[] = {
	PHP_ME(YP, __construct, arginfo_yp_class_construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(YP, getDomain, arginfo_yp_class_getDomain, ZEND_ACC_PUBLIC)
	PHP_ME(YP, setDomain, arginfo_yp_class_setDomain, ZEND_ACC_PUBLIC)
	PHP_ME(YP, cat, arginfo_yp_class_cat, ZEND_ACC_PUBLIC)
	PHP_ME(YP, match, arginfo_yp_class_match, ZEND_ACC_PUBLIC)
	PHP_ME(YP, first, arginfo_yp_class_first, ZEND_ACC_PUBLIC)
	PHP_ME(YP, next, arginfo_yp_class_next, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

static const zend_function_entry yp_YPException_methods[] = {
	PHP_FE_END
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
		RETURN_FALSE;
	}

	RETVAL_STRINGL(outval, outvallen, 1);
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
		RETURN_FALSE;
	}

	array_init(return_value);
	add_assoc_stringl_ex(return_value, outkey, outkey_len + 1, outval, outval_len, 1);
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
		RETURN_FALSE;
	}

	array_init(return_value);
	add_assoc_stringl_ex(return_value, outkey, outkey_len + 1, outval, outval_len, 1);
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

	REGISTER_LONG_CONSTANT("YPERR_ACCESS", YPERR_BADARGS, CONST_CS | CONST_PERSISTENT);
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

	zend_class_entry ce;
	INIT_NS_CLASS_ENTRY(ce, "NIS", "YP", yp_YP_methods);
	yp_ce_YP = zend_register_internal_class(&ce TSRMLS_CC);
	zend_declare_property_null(yp_ce_YP, "domain", sizeof("domain") - 1, ZEND_ACC_PROTECTED TSRMLS_CC);

	INIT_NS_CLASS_ENTRY(ce, "NIS", "YPException", yp_YPException_methods);
	yp_ce_YPException = zend_register_internal_class_ex(&ce, spl_ce_RuntimeException, NULL TSRMLS_CC);


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
