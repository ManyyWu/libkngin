#include "define.h"
#include "tcp_server.h"
#include "common.h"
#include "logfile.h"

__NAMESPACE_BEGIN

tcp_server::tcp_server (address &&_listen_addr, const tcp_server_opts &_opts)
{

}

__NAMESPACE_END

// signal ergent
// adjust size of epoll_event_set
// wait for looping