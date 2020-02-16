#ifdef _WIN32
#include "core/base/common.h"
#include "core/base/system_error.h"

#define KNGIN_FILENAME "libkngin/core/event/win_utils.cpp"

KNGIN_NAMESPACE_K_BEGIN

bool g_win_inited = false;

extern bool g_have_get_iocp_status_ex = false;

bool
wsa_init ()
{
    if (!g_win_inited) {
        WSADATA _data;
        return (!::WSAStartup(MAKEWORD(2, 2), &_data));
    }
    return true;
}

bool
wsa_deinit ()
{
    if (g_win_inited) {
        return (!::WSACleanup());
    }
    return true;
}

bool
win_api_init ()
{
    g_have_get_iocp_status_ex = true;
    return true;   
}

bool
win_init_once ()
{
    return (wsa_init() && win_api_init());
}

KNGIN_NAMESPACE_K_END

#endif /* _WIN32 */
