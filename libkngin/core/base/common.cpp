#include "core/base/common.h"

#define KNGIN_FILENAME "libkngin/core/base/common.cpp"

KNGIN_NAMESPACE_K_BEGIN

std::atomic_bool g_WSA_inited = false;

inline 
void
WSA_init ()
{
    if (!g_WSA_inited) {
        WSADATA _WSA_data;
        check(0 == ::WSAStartup(MAKEWORD(2, 2), &_WSA_data));
        g_WSA_inited = true;
    }
}


KNGIN_NAMESPACE_K_END
