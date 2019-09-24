#ifndef _COMMON_H_
#define _COMMON_H_

#include <new>
#include "logfile.h"
#include "define.h"
#include "assert.h"

__NAMESPACE_BEGIN

using std::nothrow;

#ifdef NDEBUG
template <typename __T>
__T *
new_debug (size_t _n = 1)
{
    static std::atomic<size_t> _serial(0);

    __T *_mem = NULL;
    if (1 == _n) {
        _mem = new(std::nothrow) __T;
        memory_debug("serial: %16lx, new(std::nothrow) %s, size: %ldByte",
                     typeid(__T), sizeof(__T) * _n, _serial);
    } else {
        _mem = new(std::nothrow) __T[_n];
        memory_debug("serial: %16ld, new(std::nothrow) %s[%d], size: %ldByte",
                     typeid(__T), _n, sizeof(__T) * _n, _serial);
    }
    assert(_mem);
    if (_mem)
        ++_serial;

    return _mem;
}

template <typename __T>
void
delete_debug ()
{

}

template <typename __T>
void
delete_array_debug ()
{

}

#define new_nothrow(__t, __n)   new_debug((__t), (__n))
#define safe_release(__p)       do { delete (__p); (__p) = NULL; } while (false)
#define new_nothrow_array(__t)  new(std::nothrow) __t
#define safe_release_array(__p) do { delete[] (__p); (__p) = NULL; } while (false)
#else
#define new_nothrow(__t)        new(std::nothrow) __t
#define safe_release(__p)       do { delete (__p); (__p) = NULL; } while (false)
#define new_nothrow_array(__t)  new(std::nothrow) __t
#define safe_release_array(__p) do { delete[] (__p); (__p) = NULL; } while (false)
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