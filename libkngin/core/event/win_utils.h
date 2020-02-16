#ifndef KNGIN_WIN_UTILS_H
#define KNGIN_WIN_UTILS_H

#ifdef _WIN32
#include "core/base/define.h"

KNGIN_NAMESPACE_K_BEGIN

extern bool g_win_inited;

extern bool g_have_get_iocp_status_ex;

bool
wsa_init ();

bool
wsa_deinit ();

bool
win_api_init ();

bool
win_init_once ();

KNGIN_NAMESPACE_K_END

#endif /* _WIN32 */

#endif /* KNGIN_WIN_UTILS_H */
