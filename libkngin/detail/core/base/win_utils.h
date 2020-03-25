#ifndef KNGIN_WIN_UTILS_H
#define KNGIN_WIN_UTILS_H

#include "kngin/core/define.h"
#if defined(KNGIN_SYSTEM_WIN32)

#include <WinSock2.h>
#include <Windows.h>

KNGIN_NAMESPACE_GROBLE_BEGIN

extern const LONGLONG g_monotonic_base;

#if defined(KNGIN_USE_IOCP_REACTOR)
extern bool g_have_get_iocp_status_ex;
#endif /* defined(KNGIN_USE_IOCP_REACTOR) */

KNGIN_NAMESPACE_GROBLE_END

KNGIN_NAMESPACE_DETAIL_BEGIN

#if !defined(KNGIN_USE_WINDOWS_GETTICKCOUNT)
LONGLONG
get_monotonic_base ();
#endif /* !defined(KNGIN_USE_WINDOWS_GETTICKCOUNT) */

KNGIN_NAMESPACE_DETAIL_END

#endif /* defined(KNGIN_SYSTEM_WIN32) */

#endif /* KNGIN_WIN_UTILS_H */