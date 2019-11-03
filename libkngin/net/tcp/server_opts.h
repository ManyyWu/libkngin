#ifndef _TCP_SERVER_OPTS_H_
#define _TCP_SERVER_OPTS_H_

#include "define.h"
#include "socket.h"

__NAMESPACE_BEGIN

struct tcp_server_opts {
    bool opt_allow_ipv6;
};

struct udp_server_opts {
    bool opt_allow_ipv6;
    bool max_connection;
    bool thread_num;
};

__NAMESPACE_END

#endif /* _TCP_SERVER_OPTS_H_ */