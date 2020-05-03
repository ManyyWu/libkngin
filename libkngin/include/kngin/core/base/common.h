#ifndef KNGIN_COMMON_H
#define KNGIN_COMMON_H

#include "kngin/core/define.h"
#include "kngin/core/base/log.h"
#include <cassert>

namespace k {

// exception
#define TRY() try {
#define IGNORE_EXCP(message, ...) \
    } catch (...) { /*assert(!"ignore exception");*/ log_error(message, ##__VA_ARGS__); }
#define CATCH_FATAL(message, ...) \
    } catch (...) { /*assert(!"exception");*/ log_fatal(message, ##__VA_ARGS__); throw; }
#define CATCH_ERROR(message, ...) \
    } catch (...) { /*assert(!"exception");*/ log_fatal(message, ##__VA_ARGS__); throw; }

// expression check
#define if_not(exp) \
    if (!(exp) ? log_error("expression (" #exp ") is false"), assert(exp), true : false)
#define arg_check(exp, ...) \
    do { if (!(exp)) throw_exception("" ##__VA_ARGS__); } while (false)
#define return_if(exp, ...)                                                           \
    do {                                                                              \
      if ((exp) ? log_error("expression (" #exp ") is false"), assert(exp), true : false) \
        return __VA_ARGS__;                                                           \
    } while (false)


// offset
#undef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE, MEMBER) __compiler_offsetof(TYPE, MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif /* defined(KNGIN_SYSTEM_WIN32) */

#if defined(KNGIN_SYSTEM_WIN32)
#define container_of(ptr, type, member) ((type *)( \
        (PCHAR)(ptr) - (ULONG_PTR)(&((type *)0)->member)))
#else
#define container_of(ptr, type, member) ({ \
        const typeof( ((type *)0)->member ) *__mptr = (ptr); \
        (type *)( (char *)__mptr - offsetof(type, member) );})
#endif /* defined(KNGIN_SYSTEM_WIN32) */
} /* namespace k */

#endif /* KNGIN_COMMON_H */
