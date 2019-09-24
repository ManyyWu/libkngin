#ifndef _COMMON_H_
#define _COMMON_H_

#include <new>
#include "logfile.h"
#include "define.h"
#include "assert.h"

__NAMESPACE_BEGIN

using std::nothrow;
#define new_nothrow(__t)        new(std::nothrow) __t
#define safe_release(__p)       do { delete (__p); (__p) = NULL; } while (false)
#define new_nothrow_array(__t)  new(std::nothrow) __t
#define safe_release_array(__p) do { delete[] (__p); (__p) = NULL; } while (false)

/*
 * Log for assert
 * These micro functions are only used for debugging.
 * These micro function will change the execution flow of the caller functions.
 *
 * __e: expression
 */
#define if_not(__e) \
        if (!(__e) ? (assert_log("if_not("#__e")"), assert((__e)), true) : false)
#define kassert(__e) \
        do { if_not(__e) (void)0; } while (false)
#define kassert_r(__e) \
        if_not(__e) return;
#define kassert_r0(__e) \
        if_not(__e) return 0;

__NAMESPACE_END

#endif /* _COMMON_H_ */