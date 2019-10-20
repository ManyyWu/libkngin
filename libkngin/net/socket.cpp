#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
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
    : filefd(_fd), m_opts(_fd)
{
}

socket::socket (INET_PROTOCOL _proto)
    : filefd(::socket(is_bits_set(_proto, 1) ? AF_INET6 : AF_INET,
                      is_bits_set(_proto, 0) ? SOCK_DGRAM : SOCK_STREAM, 0)),
      m_opts(m_fd)
{
}

socket::~socket ()
{
}

bool
socket::bind (inet_addr _addr)
{
    kassert(_addr);

    int _ret = ::bind(m_fd, _addr, sizeof(struct sockaddr));
    if (_ret < 0)
        log_error("::bind error() - %s:%d", strerror(errno), errno);
    return (_ret >= 0);
}

bool
socket::listen (int _max)
{
    kassert(_max >= 0);

    int _ret = ::listen(m_fd, _max);
    if (_ret < 0)
        log_error("::listen error() - %s:%d", strerror(errno), errno);
    return (_ret >= 0);
}

ssize_t
socket::writev (const std::vector<buffer> &_buf, size_t _n)
{
    kassert(_buf.size() < _n);

    std::vector<struct iovec> _iovec;
    _iovec.resize(_buf.size());
    for (auto _iter : _buf)
        _iovec.push_back({_iter.get().data(), _iter.size()});
    return ::writev(m_fd, _iovec.data(), _n);
}

ssize_t
socket::readv (std::vector<buffer> &_buf, size_t _n)
{
    kassert(_buf.size() < _n);

    std::vector<struct iovec> _iovec;
    _iovec.resize(_buf.size());
    for (auto _iter : _buf)
        _iovec.push_back({_iter.get().data(), _iter.size()});
    return ::writev(m_fd, _iovec.data(), _n);
}

__NAMESPACE_END
