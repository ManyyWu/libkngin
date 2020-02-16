#ifndef KNGIN_ERROR_H
#define KNGIN_ERROR_H

#ifndef WIN32
#include <cerrno>
#endif
#include "core/base/define.h"

KNGIN_NAMESPACE_K_BEGIN

enum APPLICATION_ERROR_CODE {
    APPE_SUCCESS = 0,   // success
    APPE_NOMEM   = 1,   // out of memory

    APPE_UNKNOWN = 255, // unknown error
};

#ifdef _WIN32
inline
DWORD
last_errno ()
{
    return ::GetLastError();
}
#else
inline
int
last_errno ()
{
    return (errno);
}
#endif

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_ERROR_H */