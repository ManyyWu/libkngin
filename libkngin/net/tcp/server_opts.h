#ifndef _TCP_SERVER_OPTS_H_
#define _TCP_SERVER_OPTS_H_

#include <string>
#include "define.h"
#include "socket.h"

__NAMESPACE_BEGIN

struct tcp_server_opts {
    std::string address;
    uint16_t    port;
    bool        allow_ipv6;
    uint16_t    thread_num;
};

struct udp_server_opts {
    bool allow_ipv6;
    bool thread_num;
};

__NAMESPACE_END

#endif /* _TCP_SERVER_OPTS_H_ */