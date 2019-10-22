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
    : filefd(_fd), m_opts(_fd)
{
}

socket::socket (INET_PROTOCOL _proto)
    : filefd(::socket(is_bits_set(_proto, 1) ? AF_INET6 : AF_INET,
                      is_bits_set(_proto, 0) ? SOCK_DGRAM : SOCK_STREAM, 0)),
      m_opts(m_fd)
{
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
