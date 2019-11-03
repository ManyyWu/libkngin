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
#include <vector>
#include "define.h"
#include "error.h"
#include "filefd.h"
#include "logfile.h"
#include "buffer.h"
#include "socket.h"
#include "address.h"
#include "event_loop.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin/net/socket.cpp"

__NAMESPACE_BEGIN

socket::socket (int _fd)
    : filefd(_fd)
{
    //log_debug("accepted a socket, fd = %d", m_fd);
}

socket::socket (INET_PROTOCOL _proto)
    : filefd(::socket(is_bits_set(_proto, 1) ? AF_INET6 : AF_INET,
                      is_bits_set(_proto, 0) ? SOCK_DGRAM : SOCK_STREAM, 0))
{
    //log_debug("new socket, fd = %d", m_fd);
}

socket::socket (socket &&_s)
    : filefd(_s.m_fd)
{
    _s.m_fd = __INVALID_FD;
}

void
socket::rd_shutdown ()
{ 
    int _ret = ::shutdown(m_fd, SHUT_RD); 
    if (_ret < 0)
        log_error("::shutdown(SHUT_RD) error - %s:%d", strerror(errno), errno);
}

void
socket::wr_shutdown ()
{ 
    int _ret = ::shutdown(m_fd, SHUT_WR); 
    if (_ret < 0)
        log_error("::shutdown(SHUT_WR) error - %s:%d", strerror(errno), errno);
}

ssize_t
socket::send (buffer &_buf, size_t _nbytes, int _flags)
{
    check(_buf.readable(_nbytes));
    ssize_t _size = ::send(m_fd, _buf.data(), _nbytes, _flags);
    if (_nbytes > 0)
        _buf.rreset(_buf.rindex() + _nbytes);
    return _size;
}

ssize_t
socket::recv (buffer &_buf, size_t _nbytes, int _flags)
{
    check(_buf.readable(_nbytes));
    ssize_t _size = ::recv(m_fd, _buf.data(), _nbytes, _flags);
    if (_nbytes > 0)
        _buf.wreset(_buf.windex() + _nbytes);
    return _size;
}

ssize_t
socket::sendto (const address &_addr, buffer &_buf, size_t _nbytes, int _flags)
{
    check(_buf.readable(_nbytes));
    ssize_t _size = ::sendto(m_fd, (const char *)_buf.data(), _nbytes, _flags,
                    (const sockaddr *)&(_addr.sa()), 
                    _addr.inet6() ? sizeof(_addr.sa().sa_in6) : sizeof(_addr.sa().sa_in));
    if (_size > 0)
        _buf.rreset(_buf.rindex() + _size);
    return _size;
};

ssize_t
socket::recvfrom (address &_addr, buffer &_buf, size_t _nbytes, int _flags)
{
    check(_buf.writeable(_nbytes));
    socklen_t _addr_len = (_addr.inet6() ? sizeof(_addr.sa().sa_in6) : sizeof(_addr.sa().sa_in));
    check(_buf.writeable() >= _nbytes);
    ssize_t _size = ::recvfrom(m_fd, (char *)_buf.data(), _nbytes, _flags,
                              (sockaddr *)&(_addr.sa()), 
                              &_addr_len);
    if (_size > 0)
        _buf.wreset(_buf.windex() + _size);
    return _size;
}

__NAMESPACE_END
