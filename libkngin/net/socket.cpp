#ifdef _WIN32
#else
#include <socket.h>
#endif
#include <memory>
#include <vector>
#include "filefd.h"
#include "buffer.h"
#include "socket.h"
#include "address.h"
#include "exception.h"
#include "event_loop.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/socket.cpp"

KNGIN_NAMESPACE_K_BEGIN

socket::socket (int _fd)
    : filefd(_fd)
{
    if (fd_invalid(m_fd)) {
        log_error("::socket() error - invalid file descriptor");
        throw k::exception("socket::socket() error");
    }
}

socket::socket (INET_PROTOCOL _proto)
    : filefd(::socket(is_bits_set(_proto, 1) ? AF_UNSPEC : AF_INET,
                      is_bits_set(_proto, 0) ? SOCK_DGRAM : SOCK_STREAM, 0))
{
    if (fd_invalid(m_fd)) {
        log_error("::socket() error - %s:%d", strerror(errno), errno);
        throw k::exception("socket::socket() error");
    }
}

socket::socket (socket &&_s)
    : filefd(_s.m_fd)
{
    _s.m_fd = filefd::invalid_fd;
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
                             (const ::sockaddr *)&(_addr.sa()),
                             _addr.inet6() ? sizeof(_addr.sa().v6) : sizeof(_addr.sa().v4));
    if (_size > 0)
        _buf.rreset(_buf.rindex() + _size);
    return _size;
};

ssize_t
socket::recvfrom (address &_addr, buffer &_buf, size_t _nbytes, int _flags)
{
    check(_buf.writeable(_nbytes));
    socklen_t _addr_len = (_addr.inet6() ? sizeof(_addr.sa().v6) : sizeof(_addr.sa().v4));
    check(_buf.writeable() >= _nbytes);
    ssize_t _size = ::recvfrom(m_fd, (char *)_buf.data(), _nbytes, _flags,
                               (::sockaddr *)&(_addr.sa()),
                               &_addr_len);
    if (_size > 0)
        _buf.wreset(_buf.windex() + _size);
    return _size;
}

KNGIN_NAMESPACE_K_END
