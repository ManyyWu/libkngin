#ifndef KNGIN_ERROR_H
#define KNGIN_ERROR_H

#ifdef WIN32
#include <Windows.h>
#else
#include <cerrno>
#endif
#include "core/base/define.h"

KNGIN_NAMESPACE_K_BEGIN

enum APPLICATION_ERROR_CODE {
    APPE_SUCCESS = 0,   // success
    APPE_NOMEM   = 1,   // out of memory

    APPE_UNKNOWN = 255, // unknown error
};

inline
uint32_t
last_errno ()
{
#ifdef _WIN32
    return ::GetLastError());
#else
    return (errno);
#endif
}

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_ERROR_H */