#ifndef _TCP_SERVER_OPTS_H_
#define _TCP_SERVER_OPTS_H_

#include <string>
#include "core/define.h"
#include "net/socket.h"

#define DEFAULT_BACKLOG 10

KNGIN_NAMESPACE_K_BEGIN
KNGIN_NAMESPACE_TCP_BEGIN

struct server_opts {
    std::string name;
    uint16_t    port;
    bool        allow_ipv6             = false;
    int         backlog                = DEFAULT_BACKLOG;
    uint16_t    thread_num             = 1;
    bool        disable_debug          = false;
    bool        disable_info           = false;
    bool        separate_listen_thread = false;
    bool        keep_alive             = false;
    bool        enable_et_mode         = false;
};

KNGIN_NAMESPACE_TCP_END
KNGIN_NAMESPACE_K_END

#endif /* _TCP_SERVER_OPTS_H_ */