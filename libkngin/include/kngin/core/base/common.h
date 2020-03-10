#ifndef KNGIN_COMMON_H
#define KNGIN_COMMON_H

#include "kngin/core/define.h"
#include "kngin/core/base/log.h"
#include <cassert>

KNGIN_NAMESPACE_K_BEGIN

// exception
#define TRY() \
    try {
#define IGNORE() \
    } catch (...) { assert(0); }
#define CATCH_FATAL(message, ...) \
    } catch (...) { assert(0); error(message, ##__VA_ARGS__); }
#define CATCH_ERROR(message, ...) \
    } catch (...) { assert(0); error(message, ##__VA_ARGS__); throw; }

// expression check
#define if_not(exp) \
    if (!(exp) ? error("expression (" #exp ") is false"), assert(exp), true : false)
#define arg_check(exp, ...) \
    do { if (!(exp)) throw_exception("" ##__VA_ARGS__); } while (false)
#define return_if(exp, ...)                                                           \
    do {                                                                              \
      if ((exp) ? error("expression (" #exp ") is false"), assert(exp), true : false) \
        return __VA_ARGS__;                                                         \
    } while (false)


KNGIN_NAMESPACE_K_END

#endif /* KNGIN_COMMON_H */
