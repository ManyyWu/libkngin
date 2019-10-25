#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/uio.h>
#endif
#include <memory>
#include <cstring>
#include <vector>
#include "define.h"
#include "filefd.h"
#include "logfile.h"
#include "buffer.h"
#include "socket.h"
#include "address.h"
#include "event_loop.h"

__NAMESPACE_BEGIN

socket::socket (int _fd)
    : filefd(_fd)
{
}

socket::socket (INET_PROTOCOL _proto)
    : filefd(::socket(is_bits_set(_proto, 1) ? AF_INET6 : AF_INET,
                      is_bits_set(_proto, 0) ? SOCK_DGRAM : SOCK_STREAM, 0))
{
}

socket::socket (socket &&_s)
    : filefd(_s.m_fd)
{
    _s.m_fd = -1;
}

__NAMESPACE_END
