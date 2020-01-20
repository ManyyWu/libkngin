#ifndef KNGIN_ERROR_H
#define KNGIN_ERROR_H

#include <system_error>
#include "core/base/define.h"

KNGIN_NAMESPACE_K_BEGIN

enum APPLICATION_ERROR_CODE {
    APPE_SUCCESS = 0,   // success
    APPE_NOMEM   = 1,   // out of memory

    APPE_UNKNOWN = 255, // unknown error
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_ERROR_H */