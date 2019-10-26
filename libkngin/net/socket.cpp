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

ssize_t
socket::sendto (const address &_addr, const buffer &_buf, size_t _nbytes, int _flags)
{
    kassert(_buf.size() >= _nbytes);
    return ::sendto(m_fd, (const char *)_buf.get().data(), _nbytes, _flags, 
                    (const sockaddr *)&(_addr.sa()), 
                    _addr.inet6() ? sizeof(_addr.sa().sa_in6) : sizeof(_addr.sa().sa_in));
};

ssize_t
socket::recvfrom (address &_addr, buffer &_buf, size_t _nbytes, int _flags)
{
    int _addr_len = (_addr.inet6() ? sizeof(_addr.sa().sa_in6) : sizeof(_addr.sa().sa_in));
    kassert(_buf.size() >= _nbytes);
    ssize_t _ret = ::recvfrom(m_fd, (char *)_buf.get().data(), _nbytes, _flags, 
                              (sockaddr *)&(_addr.sa()), 
                              &_addr_len);
    kassert(_addr_len);
}


__NAMESPACE_END
