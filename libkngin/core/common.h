#ifndef _COMMON_H_
#define _COMMON_H_

#include <new>
#include <mutex>
#include "logfile.h"
#include "define.h"
#include "assert.h"

__NAMESPACE_BEGIN

using std::nothrow;

#ifndef NDEBUG

extern size_t     __memory_debug_total;
extern void *     __memory_debug_addr;
extern std::mutex __memory_debug_mutex;

#define __new_debug_array(__t, __n)                                                                                         \
            (                                                                                                               \
             __memory_debug_mutex.lock(),                                                                                   \
             __memory_debug_addr = new(std::nothrow) __t[__n],                                                              \
             logger()[k::__LOG_FILE_MEMORY]                                                                                 \
                 ->debug("new    addr: %#016lx, total: %#016lxByte, size: %#016lxByte, new(std::nothrow) %s[%d], %s[%s:%d]",\
                         __memory_debug_addr,                                                                               \
                         (__memory_debug_total += sizeof(__t) * (__n)),                                                     \
                         sizeof(__t) * (__n), #__t, (__n),                                                                  \
                         __FUNCTION__, __FILE__, __LINE__),                                                                 \
             __memory_debug_mutex.unlock(),                                                                                 \
             (__t *)__memory_debug_addr                                                                                     \
             )

#define __new_debug(__t, __e)                                                                                               \
            (                                                                                                               \
             __memory_debug_mutex.lock(),                                                                                   \
             __memory_debug_addr = new(std::nothrow) __t __e,                                                              \
             logger()[k::__LOG_FILE_MEMORY]                                                                                 \
                 ->debug("new    addr: %#016lx, total: %#016lxByte, size: %016lxByte, new(std::nothrow) %s(%s), %s[%s:%d]", \
                         __memory_debug_addr,                                                                               \
                         (__memory_debug_total += sizeof(__t)), sizeof(__t), #__t, #__e,                                    \
                         __FUNCTION__, __FILE__, __LINE__),                                                                 \
             __memory_debug_mutex.unlock(),                                                                                 \
             (__t *)__memory_debug_addr                                                                                     \
             )

#define __delete_debug_array(__p)                              \
            do {                                               \
                __memory_debug_mutex.lock();                   \
                logger()[k::__LOG_FILE_MEMORY]                 \
                    ->debug("delete addr: %#016lx, %s[%s:%d]", \
                            __memory_debug_addr,               \
                            __FUNCTION__, __FILE__, __LINE__), \
                delete __p;                                    \
                __p = NULL;                                    \
                __memory_debug_mutex.unlock();                 \
            } while (false)

#define __delete_debug(__p)                                    \
            do {                                               \
                __memory_debug_mutex.lock();                   \
                logger()[k::__LOG_FILE_MEMORY]                 \
                    ->debug("delete addr: %#016lx, %s[%s:%d]", \
                            __memory_debug_addr,               \
                            __FUNCTION__, __FILE__, __LINE__), \
                delete __p;                                    \
                __p = NULL;                                    \
                __memory_debug_mutex.unlock();                 \
            } while (false)

#define knew          __new_debug
#define kdelete       __delete_debug
#define knew_array    __new_debug_array
#define kdelete_array __delete_debug_array
#else
#define knew(__t, __e)       new(std::nothrow) __t(__e)
#define kdelete(__p)         do { delete (__p); (__p) = NULL; } while (false)
#define knew_array(__t, __n) new(std::nothrow) __t[__n]
#define kdelete_array(__p)   do { delete[] (__p); (__p) = NULL; } while (false)
#endif

/*
 * Log for assert
 * These micro functions are only used for debugging.
 * These micro function will change the execution flow of the caller functions.
 *
 * __e: expression
 */
#define if_not(__e) \
        if (!(__e) ? (assert_log(expression (__e) is false), assert((__e)), true) : false)
#define kassert(__e) \
        do { if_not(__e) (void)0; } while (false)
#define kassert_r(__e) \
        if_not(__e) return;
#define kassert_r0(__e) \
        if_not(__e) return 0;

__NAMESPACE_END

#endif /* _COMMON_H_ */