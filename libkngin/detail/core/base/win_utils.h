#ifndef KNGIN_WIN_UTILS_H
#define KNGIN_WIN_UTILS_H

#include "kngin/core/define.h"
#if defined(KNGIN_SYSTEM_WIN32)

#include <WinSock2.h>
#include <Windows.h>

namespace {

extern const LONGLONG g_monotonic_base;

#if defined(KNGIN_USE_IOCP_REACTOR)
extern bool g_have_get_iocp_status_ex;
#endif /* defined(KNGIN_USE_IOCP_REACTOR) */

}

namespace k::detail {

#if !defined(KNGIN_USE_WINDOWS_GETTICKCOUNT)
LONGLONG
get_monotonic_base ();
#endif /* !defined(KNGIN_USE_WINDOWS_GETTICKCOUNT) */

} /* namespace k::detail */

#endif /* defined(KNGIN_SYSTEM_WIN32) */

#endif /* KNGIN_WIN_UTILS_H */