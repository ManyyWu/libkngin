#ifndef _COMMON_H_
#define _COMMON_H_

#include <new>
#include "define.h"
#include "assert.h"

#define __log_assert_format(__f) "****** %s[%s:%d] " __f " *******"

__NAMESPACE_BEGIN

using std::nothrow;
#define new_nothrow(__t)        new(std::nothrow) __t
#define safe_release(__p)       do { delete (__p); (__p) = NULL; } while (false)
#define new_nothrow_array(__t)  new(std::nothrow) __t
#define safe_release_array(__p) do { delete[] (__p); (__p) = NULL; } while (false)

#ifndef NDEBUG
void
__log_assert            (const char *_fmt, ...);

void
__log_assert_dump_stack (const char *_fmt, ...);

/*
 * Log for assert
 * These micro functions are only used for debugging
 * and only be called at the beginning of the function.
 * These micro function will change the execution flow
 * of the caller functions.
 *
 * __e: expression
 */
#define if_not(__e)                                   \
        if (!(__e)                                    \
            ? (__log_assert(__log_assert_format("%s"),\
                           __FUNCTION__,              \
                           __FILE__,                  \
                           __LINE__,                  \
                           "if_not("#__e")"           \
                           ),                         \
               assert((__e)),                         \
               true)                                  \
            : false                                   \
        )
#define kassert(__e)    do { if_not(__e) (void)0; } while (false)
#define kassert_r(__e)  if_not(__e) return;
#define kassert_r0(__e) if_not(__e) return 0;

//#define kassert(__e)                                    \
//        do {                                            \
//            if (!(__e)) {                               \
//                assert((__e));                          \
//                __log_assert(__log_assert_format("%s"), \
//                             __FUNCTION__,              \
//                             __FILE__,                  \
//                             __LINE__,                  \
//                             "kassert("#__e")"          \
//                             );                         \
//            }                                           \
//        } while (false)
//
//
//#define kassert_ret(__e)                                \
//        do {                                            \
//            if (!(__e)) {                               \
//                assert((__e));                          \
//                __log_assert(__log_assert_format("%s"), \
//                             __FUNCTION__,              \
//                             __FILE__,                  \
//                             __LINE__,                  \
//                             "kassert_ret("#__e")"      \
//                             );                         \
//                return;                                 \
//            }                                           \
//        } while (false)
//
//#define kassert_ret0(__e)                               \
//        do {                                            \
//            if (!(__e)) {                               \
//                assert((__e));                          \
//                __log_assert(__log_assert_format("%s"), \
//                             __FUNCTION__,              \
//                             __FILE__,                  \
//                             __LINE__,                  \
//                             "kassert_ret0("#__e")"     \
//                             );                         \
//                return 0;                               \
//            }                                           \
//        } while (false)
//
//#define kassert_dump(__e)                                          \
//        do {                                                       \
//            if (!(__e)) {                                          \
//                assert((__e));                                     \
//                __log_assert_dump_stack(__log_assert_format("%s"), \
//                                        __FUNCTION__,              \
//                                        __FILE__,                  \
//                                        __LINE__,                  \
//                                        "kassert_dump("#__e")"     \
//                                        );                         \
//            }                                                      \
//        } while (false)
//
//#define kassert_dump_ret(__e)                                      \
//        do {                                                       \
//            if (!(__e)) {                                          \
//                assert((__e));                                     \
//                __log_assert_dump_stack(__log_assert_format("%s"), \
//                                        __FUNCTION__,              \
//                                        __FILE__,                  \
//                                        __LINE__,                  \
//                                        "kassert_dump_ret("#__e")" \
//                                        );                         \
//                return;                                            \
//            }                                                      \
//        } while (false)
//
//#define kassert_dump_ret0(__e)                                      \
//        do {                                                        \
//            if (!(__e)) {                                           \
//                assert((__e));                                      \
//                __log_assert_dump_stack(__log_assert_format("%s"),  \
//                                        __FUNCTION__,               \
//                                        __FILE__,                   \
//                                        __LINE__,                   \
//                                        "kassert_dump_ret0("#__e")" \
//                                        );                          \
//                return 0;                                           \
//            }                                                       \
//        } while (false)
//
#else /* NDEBUG */
//#define kassert(__e)           ((void)0)
//#define kassert_ret(__e)       ((void)0)
//#define kassert_ret0(__e)      ((void)0)
//#define kassert_dump(__e)      ((void)0)
//#define kassert_ret_dump(__e)  ((void)0)
//#define kassert_ret0_dump(__e) ((void)0)
#endif /* NDEBUG */

__NAMESPACE_END

#endif /* _COMMON_H_ */