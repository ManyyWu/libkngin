#ifndef _COMMON_H_
#define _COMMON_H_

#include "define.h"
#include "assert.h"

#define __log_assert_format(__f) "****** %s[%s:%d] " __f " *******"

__NAMESPACE_BEGIN

//#ifndef NDEBUG
//void
//__log_assert            (const char *_fmt, ...);
//
//void
//__log_assert_dump_stack (const char *_fmt, ...);

/*
 * Log for assert
 * These micro functions are only used for debugging
 * and only be called at the beginning of the function.
 * These micro function will change the execution flow
 * of the caller functions.
 */
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
//#else /* NDEBUG */
//#define kassert(__e)           ((void)0)
//#define kassert_ret(__e)       ((void)0)
//#define kassert_ret0(__e)      ((void)0)
//#define kassert_dump(__e)      ((void)0)
//#define kassert_ret_dump(__e)  ((void)0)
//#define kassert_ret0_dump(__e) ((void)0)
//#endif /* NDEBUG */

__NAMESPACE_END

#endif /* _COMMON_H_ */