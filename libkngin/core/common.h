#ifndef _COMMON_H_
#define _COMMON_H_

#define __STDC_FORMAT_MACROS
#include <new>
#include <cinttypes>
#include <cassert>
#include "core/exception.h"
#include "core/define.h"
#include "core/logfile.h"

KNGIN_NAMESPACE_K_BEGIN

#define safe_release(ptr)       do { delete (ptr); (ptr) = nullptr; } while (false)
#define safe_release_array(ptr) do { delete [] (ptr); (ptr) = nullptr; } while (false)

/*
 * Log for assert
 * These micro functions are only used for debugging.
 * These micro function will change the execution flow of the caller functions.
 *
 * exp: expression
 */
#define arg_check(exp, what) do { if (!(exp) ? assert((exp)), true : false) throw k::exception(what " - invalid argument"); } while (false)
#define if_not(exp)          if (!(exp) ? (assert_log(expression (exp) is false), assert((exp)), true) : false)
#define check(exp)           do { if_not(exp) (void)0; } while (false)
#define ignore_exp(exp)      try { {exp;} } catch (...) { log_warning("caught an exception be ignored"); }

/*
* nullptr reference
*/
template <typename Type>
Type &
nullptr_ref ()
{
    return *static_cast<Type *>(nullptr);
}

template <typename Type>
bool
is_nullptr_ref (Type &_ref)
{
    return (nullptr == _ref);
}

KNGIN_NAMESPACE_K_END

#endif /* _COMMON_H_ */