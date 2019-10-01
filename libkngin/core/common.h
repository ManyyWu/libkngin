#ifndef _COMMON_H_
#define _COMMON_H_

#include <new>
#include <cstdio>
#include <atomic>
#include "logfile.h"
#include "define.h"
#include "assert.h"

__NAMESPACE_BEGIN

using std::nothrow;

#ifdef NDEBUG

extern std::atomic<size_t> __g_memory_debug_total;

#define __new_debug_array(__p, __t, __n)                                                                                           \
            do {                                                                                                                   \
                (__p) = new(std::nothrow) __t[(__n)];                                                                              \
                size_t __memory_debug_total = (__g_memory_debug_total += sizeof(__t) * (__n));                                     \
                if (logger()[k::__LOG_FILE_MEMORY])                                                                                \
                    logger()[k::__LOG_FILE_MEMORY]                                                                                 \
                        ->debug("new    addr: %#016lx, total: %#016lxByte, size: %#016lxByte, new(std::nothrow) %s[%d], %s[%s:%d]",\
                                (unsigned long)__p, __memory_debug_total, sizeof(__t) * (__n), #__t, ((int)__n),                            \
                                __FUNCTION__, __FILE__, __LINE__);                                                                 \
            } while (false)

#define __new_debug(__p, __t, __e)                                                                                                 \
            do {                                                                                                                   \
                (__p) = new(std::nothrow) __t __e;                                                                                 \
                size_t __memory_debug_total = (__g_memory_debug_total += sizeof(__t));                                             \
                if (logger()[k::__LOG_FILE_MEMORY])                                                                                \
                    logger()[k::__LOG_FILE_MEMORY]                                                                                 \
                        ->debug("new    addr: %#016lx, total: %#016lxByte, size: %#016lxByte, new(std::nothrow) %s%s, %s[%s:%d]",  \
                                (unsigned long)__p, __memory_debug_total , sizeof(__t), #__t, #__e,                                         \
                                __FUNCTION__, __FILE__, __LINE__);                                                                 \
            } while (false)

#define __delete_debug_array(__p)                                         \
            do {                                                          \
                if (logger()[k::__LOG_FILE_MEMORY]) {                     \
                    logger()[k::__LOG_FILE_MEMORY]                        \
                        ->debug("delete addr: %#016lx, %s[%s:%d]",        \
                                (unsigned long)__p, __FUNCTION__, __FILE__, __LINE__); \
                } else {                                                  \
                    fprintf(stderr, "logger uninted, unrecorded log: "    \
                            "\"delete addr: %#016lx, %s[%s:%d]\"\n",      \
                            (unsigned long)__p, __FUNCTION__, __FILE__, __LINE__); \
                }                                                         \
                delete (__p);                                             \
                (__p) = NULL;                                             \
            } while (false)

#define __delete_debug(__p)                                               \
            do {                                                          \
                if (logger()[k::__LOG_FILE_MEMORY]) {                     \
                    logger()[k::__LOG_FILE_MEMORY]                        \
                        ->debug("delete addr: %#016lx, %s[%s:%d]",        \
                                (unsigned long)__p, __FUNCTION__, __FILE__, __LINE__); \
                } else {                                                  \
                    fprintf(stderr, "logger uninted, unrecorded log: "    \
                            "\"delete addr: %#016lx, %s[%s:%d]\"\n",      \
                            (unsigned long)__p, __FUNCTION__, __FILE__, __LINE__); \
                }                                                         \
                delete (__p);                                             \
                (__p) = NULL;                                             \
            } while (false)

#define __delete_debug_this(__p)                                          \
            do {                                                          \
                if (logger()[k::__LOG_FILE_MEMORY]) {                     \
                    logger()[k::__LOG_FILE_MEMORY]                        \
                        ->debug("delete addr: %#016lx, %s[%s:%d]",        \
                                (unsigned) __p, __FUNCTION__, __FILE__, __LINE__); \
                } else {                                                  \
                    fprintf(stderr, "logger uninted, unrecorded log: "    \
                            "\"delete addr: %#016lx, %s[%s:%d]\"\n",      \
                            (unsigned) __p, __FUNCTION__, __FILE__, __LINE__);     \
                }                                                         \
                delete (__p);                                             \
            } while (false)
// Ö±½ÓÊä³öµ½stderr
#define knew          __new_debug
#define kdelete       __delete_debug
#define kdelete_this  __delete_debug_this
#define knew_array    __new_debug_array
#define kdelete_array __delete_debug_array
#else
#define knew(__p, __t, __e)       (__p) = new(std::nothrow) __t __e
#define kdelete(__p)              do { delete (__p); (__p) = NULL; } while (false)
#define knew_array(__p, __t, __n) (__p) = new(std::nothrow) __t[__n]
#define kdelete_array(__p)        do { delete[] (__p); (__p) = NULL; } while (false)
#define kdelete_this(__p)         delete (this)
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