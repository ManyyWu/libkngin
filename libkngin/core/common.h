#ifndef _COMMON_H_
#define _COMMON_H_

#define __STDC_FORMAT_MACROS
#include <new>
#include <cinttypes>
#include "define.h"
#include "logfile.h"
#include "assert.h"

__NAMESPACE_BEGIN

#define safe_release(__p)         do { delete (__p); (__p) = nullptr; } while (false)
#define safe_release_array(__p)   do { delete [] (__p); (__p) = nullptr; } while (false)

/*
 * Log for assert
 * These micro functions are only used for debugging.
 * These micro function will change the execution flow of the caller functions.
 *
 * __e: expression
 */
#define arg_check(__e) if (!(__e) ? (assert_log(invalid argument, expression (__e) is false), assert((__e)), true) : false)
#define if_not(__e)    if (!(__e) ? (assert_log(expression (__e) is false), assert((__e)), true) : false)
#define check(__e)     do { if_not(__e) (void)0; } while (false)
#define check_r(__e)   if_not(__e) return
#define check_r0(__e)  if_not(__e) return 0

/*
* nullptr reference
*/
template <typename __T>
__T &
nullptr_ref ()
{
    return *static_cast<__T *>(nullptr);
}

template <typename __T>
bool
is_nullptr_ref (__T &_ref)
{
    return (nullptr == _ref);
}

__NAMESPACE_END

#endif /* _COMMON_H_ */