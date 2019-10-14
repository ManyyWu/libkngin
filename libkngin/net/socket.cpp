#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif
#include <memory>
#include <cstring>
#include "define.h"
#include "filefd.h"
#include "logfile.h"
#include "socket.h"
#include "event_loop.h"

__NAMESPACE_BEGIN

socket::socket (event_loop *_loop, INET_PROTOCOL _protocol)
    try
    : filefd(::socket(is_bits_set(_protocol, 1) ? AF_INET6 : AF_INET,
                      is_bits_set(_protocol, 0) ? SOCK_DGRAM : SOCK_STREAM, 0)),
      m_loop(_loop), m_event(new epoller_event(_loop, this))
{
    kassert(_loop);
} catch (...) {
    log_fatal("socket::socket() error");
    throw;
}

socket::socket (INET_PROTOCOL _protocol)
    : filefd(::socket(is_bits_set(_protocol, 1) ? AF_INET6 : AF_INET,
                      is_bits_set(_protocol, 0) ? SOCK_DGRAM : SOCK_STREAM, 0)),
      m_loop(NULL), m_event(nullptr)
{
}

socket::~socket ()
{
}

bool
socket::set_reuse_addr (bool _on /* = true */)
{
    int optval = _on ? 1 : 0;
    int _ret = ::setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));
    if (_ret < 0)
        log_error("::setsockopt() set SO_REUSEADDR flag failed - %s:%d", strerror(errno), errno);
    return (_ret >= 0);
}

bool
socket::set_reuse_port (bool _on /* = true */)
{
#ifdef _WIN32
    int optval = _on ? 0 : 1;
    int _ret = ::setsockopt(m_fd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *)&optval, sizeof(optval));
    set_reuse_addr(_on);
#else
    int optval = _on ? 1 : 0;
    int _ret = ::setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT, (char *)&optval, sizeof(optval));
#endif
    if (_ret < 0)
        log_error("::setsockopt() set SO_REUSEPORT flag failed - %s:%d", strerror(errno), errno);
    return (_ret >= 0);
}

bool
socket::reuse_port () const
{
}

bool
socket::closeexec () const
{

}

bool
set_keep_alive (bool _on /* = true */)
{
}
epoller_event *
socket::get_event ()
{
    return m_event.get();
}

__NAMESPACE_END
