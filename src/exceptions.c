/*
 * exceptions.c
 *
 * See exceptions.h.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_smart_str.h"

#include "exceptions.h"

zend_class_entry *math_ce_FormatException;
zend_class_entry *math_ce_ArithmeticException;

zend_result math_exceptions_minit(void)
{
	zend_class_entry ce;

	/* Registered under the exact same fully-qualified name as the userland
	 * OceanMoon\Core\Exceptions\FormatException class, so PHP code written against the userland
	 * Core package works unchanged when this extension is loaded - same approach as Complex
	 * registering as OceanMoon\Math\Complex rather than some extension-local name. Both are trivial
	 * empty subclasses of DomainException in the PHP source, with no methods/properties of their
	 * own, so there's nothing else to generate a stub/arginfo for. */
	INIT_CLASS_ENTRY(ce, "OceanMoon\\Core\\Exceptions\\FormatException", NULL);
	math_ce_FormatException = zend_register_internal_class_ex(&ce, spl_ce_DomainException);

	INIT_CLASS_ENTRY(ce, "OceanMoon\\Core\\Exceptions\\ArithmeticException", NULL);
	math_ce_ArithmeticException = zend_register_internal_class_ex(&ce, spl_ce_DomainException);

	return SUCCESS;
}

void math_throw_overflow(const char *operation)
{
	zend_string *msg = strpprintf(0, "Cannot %s. The result would overflow.", operation);
	zend_throw_exception(spl_ce_OverflowException, ZSTR_VAL(msg), 0);
	zend_string_release(msg);
}

void math_rethrow_overflow(const char *operation)
{
	if (EG(exception) && instanceof_function(EG(exception)->ce, spl_ce_OverflowException)) {
		math_throw_overflow(operation);
	}
}

/* {{{ math_utf8_char_len
 *
 * Returns the byte length of the well-formed UTF-8 character at str[pos], or 0 if it isn't one: a truncated or
 * overlong sequence, a surrogate, or a code point above U+10FFFF. Matches mb_check_encoding($str, 'UTF-8').
 */
static size_t math_utf8_char_len(const unsigned char *str, size_t len, size_t pos)
{
	unsigned char c = str[pos];
	if (c < 0x80) {
		return 1;
	}

	size_t n;
	unsigned int min, cp;
	if ((c & 0xE0) == 0xC0) {
		n = 2;
		min = 0x80;
		cp = c & 0x1F;
	} else if ((c & 0xF0) == 0xE0) {
		n = 3;
		min = 0x800;
		cp = c & 0x0F;
	} else if ((c & 0xF8) == 0xF0) {
		n = 4;
		min = 0x10000;
		cp = c & 0x07;
	} else {
		return 0;
	}

	if (pos + n > len) {
		return 0;
	}
	for (size_t i = 1; i < n; i++) {
		unsigned char cc = str[pos + i];
		if ((cc & 0xC0) != 0x80) {
			return 0;
		}
		cp = (cp << 6) | (cc & 0x3F);
	}

	if (cp < min || cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF)) {
		return 0;
	}
	return n;
}
/* }}} */

zend_string *math_abbrev_string(const char *str, size_t len)
{
	const unsigned char *ustr = (const unsigned char *) str;
	const size_t max_len = 32;

	/* Build the quoted, escaped string, tracking where each character starts, so it can be cut by character. */
	smart_str quoted = {0};
	size_t *starts = safe_emalloc(2 * len + 3, sizeof(size_t), 0);
	size_t char_count = 0;

	starts[char_count++] = 0;
	smart_str_appendc(&quoted, '\'');
	for (size_t pos = 0; pos < len;) {
		size_t n = math_utf8_char_len(ustr, len, pos);
		if (n == 0) {
			smart_str_free(&quoted);
			efree(starts);
			return NULL;
		}
		if (str[pos] == '\\' || str[pos] == '\'') {
			starts[char_count++] = ZSTR_LEN(quoted.s);
			smart_str_appendc(&quoted, '\\');
		}
		starts[char_count++] = ZSTR_LEN(quoted.s);
		smart_str_appendl(&quoted, str + pos, n);
		pos += n;
	}
	starts[char_count++] = ZSTR_LEN(quoted.s);
	smart_str_appendc(&quoted, '\'');
	smart_str_0(&quoted);

	if (char_count <= max_len) {
		efree(starts);
		return quoted.s;
	}

	/* Keep the first max_len - 2 characters, then add "…'". Concatenated, as printf's %.*s would stop at a NUL. */
	zend_string *result = zend_string_concat2(ZSTR_VAL(quoted.s), starts[max_len - 2], "\xE2\x80\xA6'", 4);
	smart_str_free(&quoted);
	efree(starts);
	return result;
}

void math_throw_string_error(
	zend_class_entry *ce, const char *str, size_t len, const char *class_name, const char *reason
)
{
	zend_string *abbrev = math_abbrev_string(str, len);
	zend_string *msg;
	if (abbrev) {
		/* Concatenated, as printf's %s would stop at a NUL, which the string may contain. */
		zend_string *suffix = strpprintf(0, " to %s. %s", class_name, reason);
		msg = zend_string_concat3(
			ZEND_STRL("Cannot convert string "), ZSTR_VAL(abbrev), ZSTR_LEN(abbrev), ZSTR_VAL(suffix), ZSTR_LEN(suffix)
		);
		zend_string_release(suffix);
		zend_string_release(abbrev);
	} else {
		msg = strpprintf(0, "Cannot convert string to %s. %s", class_name, reason);
	}

	/* zend_throw_exception() takes a C string, which would also stop at a NUL, so set the message afterwards. */
	zend_object *ex = zend_throw_exception(ce, "", 0);
	zval message;
	ZVAL_STR(&message, msg);
	zend_update_property_ex(zend_ce_exception, ex, ZSTR_KNOWN(ZEND_STR_MESSAGE), &message);
	zend_string_release(msg);
}

void math_throw_invalid_format(const char *str, size_t len, const char *class_name)
{
	math_throw_string_error(math_ce_FormatException, str, len, class_name, "Invalid format.");
}
