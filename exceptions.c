/*
 * exceptions.c
 *
 * See exceptions.h.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "ext/spl/spl_exceptions.h"

#include "exceptions.h"

zend_class_entry *math_ce_FormatException;
zend_class_entry *math_ce_ArithmeticException;

zend_result math_exceptions_minit(void)
{
	zend_class_entry ce;

	/* Registered under the exact same fully-qualified name as the userland
	 * OceanMoon\Core\Exceptions\FormatException class, so PHP code written against the userland
	 * Core package works unchanged when this extension is loaded -- same approach as Complex
	 * registering as OceanMoon\Math\Complex rather than some extension-local name. Both are trivial
	 * empty subclasses of DomainException in the PHP source, with no methods/properties of their
	 * own, so there's nothing else to generate a stub/arginfo for. */
	INIT_CLASS_ENTRY(ce, "OceanMoon\\Core\\Exceptions\\FormatException", NULL);
	math_ce_FormatException = zend_register_internal_class_ex(&ce, spl_ce_DomainException);

	INIT_CLASS_ENTRY(ce, "OceanMoon\\Core\\Exceptions\\ArithmeticException", NULL);
	math_ce_ArithmeticException = zend_register_internal_class_ex(&ce, spl_ce_DomainException);

	return SUCCESS;
}
