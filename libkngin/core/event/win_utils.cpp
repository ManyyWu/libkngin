#ifdef _WIN32
#include "core/base/common.h"
#include "core/base/system_error.h"

#define KNGIN_FILENAME "libkngin/core/event/win_utils.cpp"

KNGIN_NAMESPACE_K_BEGIN

bool g_win_inited = false;

extern bool g_have_get_iocp_status_ex = false;

void
wsa_init ()
{
    if (!g_win_inited) {
        WSADATA data;
        cond_sys_err(
            0 != ::WSAStartup(MAKEWORD(2, 2), &data),
            "::WSAStartup() error"
        );
    }
}

void
wsa_deinit ()
{
    if (g_win_inited) {
        cond_sys_err(
            0 == !::WSACleanup(),
            "::WSACleanup() error"
        );
    }
}

void
win_api_init ()
{
    g_have_get_iocp_status_ex = true;
}

void
win_init_once ()
{
    wsa_init();
    win_api_init();
}

KNGIN_NAMESPACE_K_END

#endif /* _WIN32 */
