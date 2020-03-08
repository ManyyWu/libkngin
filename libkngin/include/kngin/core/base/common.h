#ifndef KNGIN_COMMON_H
#define KNGIN_COMMON_H

#include "kngin/core/define.h"
#include <cassert>

KNGIN_NAMESPACE_K_BEGIN

// exception
#define TRY() \
    try {
#define IGNORE() \
    } catch (...) {}
#define CATCH_FATAL(message, ...) \
    } catch (...) { error(message, ##__VA_ARGS__); }
#define CATCH_ERROR(message, ...) \
    } catch (...) { error(message, ##__VA_ARGS__); throw; }



KNGIN_NAMESPACE_K_END

#endif /* KNGIN_COMMON_H */
