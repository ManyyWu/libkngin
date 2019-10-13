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
#include "define.h"
#include "filefd.h"
#include "logfile.h"
#include "socket.h"
#include "event_loop.h"

__NAMESPACE_BEGIN

socket::socket (event_loop *_loop, INET_PROTOCOL _protocol)
    try
    : filefd(::socket(is_set(_protocol, 1) ? AF_INET6 : AF_INET,
                      is_set(_protocol, 0) ? SOCK_DGRAM : SOCK_STREAM, 0)),
      m_loop(_loop), m_event(new epoller_event(_loop, this))
{
    kassert(_loop);
} catch (...) {
    log_fatal("socket::socket() error");
    throw;
}

socket::socket (INET_PROTOCOL _protocol)
    : filefd(::socket(is_set(_protocol, 1) ? AF_INET6 : AF_INET,
      is_set(_protocol, 0) ? SOCK_DGRAM : SOCK_STREAM, 0)),
      m_loop(NULL), m_event(nullptr)
{
}

socket::~socket ()
{
}





epoller_event *
socket::get_event ()
{
    return m_event.get();
}

__NAMESPACE_END
