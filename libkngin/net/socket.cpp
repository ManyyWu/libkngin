#include "core/common.h"
#include "core/bits.h"
#include "net/socket.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/socket.cpp"

KNGIN_NAMESPACE_K_BEGIN

socket::socket (int _fd) KNGIN_EXP
    : filefd(_fd),
      m_rd_closed(FD_VALID(_fd)),
      m_wr_closed(FD_VALID(_fd))
{
    if (FD_INVALID(m_fd))
        throw k::exception("socket::socket() error - invalid file descriptor");
}

socket::socket (INET_PROTOCOL _proto) KNGIN_EXP
    try
    : filefd(::socket(is_bits_set(_proto, 1) ? AF_UNSPEC : AF_INET,
                      is_bits_set(_proto, 0) ? SOCK_DGRAM : SOCK_STREAM, 0)),
      m_rd_closed(FD_VALID(m_fd)),
      m_wr_closed(FD_VALID(m_fd))
{
    if (FD_INVALID(m_fd))
        throw k::system_error("::socket() error");
} catch (...) {
    log_error("socket::socket() error");
    throw;
}

socket::socket (socket &&_s) KNGIN_NOEXP
    : filefd(_s.m_fd),
      m_rd_closed(_s.m_rd_closed),
      m_wr_closed(_s.m_wr_closed)
{
    _s.m_fd = filefd::invalid_fd;
    _s.m_rd_closed = true;
    _s.m_wr_closed = true;
}

socket::~socket () KNGIN_NOEXP
{
    m_fd = (m_rd_closed && m_wr_closed) ? filefd::invalid_fd : m_fd;
}

void
socket::bind (const address &_addr) KNGIN_EXP
{
    assert(!m_wr_closed || !m_rd_closed);
    if (::bind(m_fd, (const ::sockaddr *)&(_addr.m_sa), _addr.size()) < 0)
        throw k::system_error("::bind() error");
}

void
socket::bind (const address &_addr, std::error_code &_ec) KNGIN_NOEXP
{
    assert(!m_wr_closed || !m_rd_closed);
    _ec = (::bind(m_fd, (const ::sockaddr *)&(_addr.m_sa), _addr.size()) < 0)
          ? last_error()
          : std::error_code();
}

void
socket::listen (int _backlog) KNGIN_EXP
{
    assert(!m_wr_closed || !m_rd_closed);
    if (::listen(m_fd, _backlog) < 0)
        throw k::system_error("::listen() error");
}

void
socket::listen (int _backlog, std::error_code &_ec) KNGIN_NOEXP
{
    assert(!m_wr_closed || !m_rd_closed);
    _ec = (::listen(m_fd, _backlog) < 0) ? last_error() : std::error_code();
}

int
socket::accept (address &_addr) KNGIN_EXP
{
    assert(!m_wr_closed || !m_rd_closed);
    socklen_t _len = sizeof(_addr.m_sa);
    int _fd = ::accept(m_fd, (::sockaddr *)&(_addr.m_sa), &_len);
    if (_fd < 0)
        throw k::system_error("::accept() error");
    return _fd;
}

int
socket::accept (address &_addr, std::error_code &_ec) KNGIN_NOEXP
{
    assert(!m_wr_closed || !m_rd_closed);
    socklen_t _len = sizeof(_addr.m_sa);
    int _fd = ::accept(m_fd, (::sockaddr *)&(_addr.m_sa), &_len);
    _ec = (_fd < 0) ? last_error() : std::error_code();
    return _fd;
}

void
socket::connect (const address &_addr) KNGIN_EXP
{
    assert(!m_wr_closed || !m_rd_closed);
    if (::connect(m_fd, (const ::sockaddr *)&(_addr.m_sa), _addr.size()) < 0)
        throw k::system_error("::connect() error");
}

void
socket::connect (const address &_addr, std::error_code &_ec) KNGIN_NOEXP
{
    assert(!m_wr_closed || !m_rd_closed);
    _ec = (::connect(m_fd, (const ::sockaddr *)&(_addr.m_sa), _addr.size()) < 0)
          ? last_error()
          : std::error_code();
}

void
socket::rd_shutdown () KNGIN_EXP
{
    assert(!m_rd_closed);
    if (::shutdown(m_fd, SHUT_RD) < 0)
        throw k::system_error("::shutdown(SHUT_RD) error");
}

void
socket::rd_shutdown (std::error_code &_ec) KNGIN_NOEXP
{
    assert(!m_rd_closed);
    _ec = (::shutdown(m_fd, SHUT_RD) < 0 ? last_error() : std::error_code());
}

void
socket::wr_shutdown () KNGIN_EXP
{ 
    assert(!m_wr_closed);
    if (::shutdown(m_fd, SHUT_WR) < 0)
        throw k::system_error("::shutdown(SHUT_WR) error");
}

void
socket::wr_shutdown (std::error_code &_ec) KNGIN_NOEXP
{
    assert(!m_wr_closed);
    _ec = (::shutdown(m_fd, SHUT_WR) < 0 ? last_error() : std::error_code());
}

size_t
socket::send (buffer &_buf, size_t _nbytes, int _flags) KNGIN_EXP
{
#warning "const buffer"
    assert(!m_wr_closed);
    check(_buf.readable(_nbytes));
    ssize_t _size = ::send(m_fd, _buf.data(), _buf.readable(_nbytes), _flags);
    if (_size < 0)
        throw k::system_error("::send() error");
    _buf.rreset(_buf.rindex() + _nbytes);
    return _size;
}

size_t
socket::send (buffer &_buf, size_t _nbytes, int _flags, std::error_code &_ec) KNGIN_NOEXP
{
#warning "const buffer"
    assert(!m_wr_closed);
    check(_buf.readable(_nbytes));
    ssize_t _size = ::send(m_fd, _buf.data(), _buf.readable(_nbytes), _flags);
    if (_size < 0) {
        _ec = last_error();
        return 0;
    } else {
        _ec = std::error_code();
    }
    _buf.rreset(_buf.rindex() + _nbytes);
    return _size;
}

size_t
socket::recv (buffer &_buf, size_t _nbytes, int _flags) KNGIN_EXP
{
    assert(!m_rd_closed);
    check(_buf.readable(_nbytes));
    ssize_t _size = ::recv(m_fd, _buf.data(), _buf.readable(_nbytes), _flags);
    if (_size < 0)
        throw k::system_error("::recv() error");
    _buf.wreset(_buf.windex() + _nbytes);
    return _size;
}

size_t
socket::recv (buffer &_buf, size_t _nbytes, int _flags, std::error_code &_ec) KNGIN_NOEXP
{
    assert(!m_rd_closed);
    check(_buf.readable(_nbytes));
    ssize_t _size = ::recv(m_fd, _buf.data(), _buf.readable(_nbytes), _flags);
    if (_size < 0) {
        _ec = last_error();
        return 0;
    } else {
        _ec = std::error_code();
    }
    _buf.wreset(_buf.windex() + _nbytes);
    return _size;
}

size_t
socket::sendto (const address &_addr, buffer &_buf, size_t _nbytes, int _flags) KNGIN_EXP
{
    assert(!m_wr_closed);
    check(_buf.readable(_nbytes));
    ssize_t _size = ::sendto(m_fd, (const char *)_buf.data(), _nbytes, _flags,
                             (const ::sockaddr *)&(_addr.sa()),
                             _addr.inet6() ? sizeof(_addr.sa().v6) : sizeof(_addr.sa().v4));
    if (_size < 0)
        throw k::system_error("::sendto() error");
    _buf.rreset(_buf.rindex() + _size);
    return _size;
}

size_t
socket::sendto (const address &_addr, buffer &_buf, size_t _nbytes, int _flags,
                std::error_code &_ec) KNGIN_NOEXP
{
    assert(!m_wr_closed);
    check(_buf.readable(_nbytes));
    ssize_t _size = ::sendto(m_fd, (const char *)_buf.data(), _nbytes, _flags,
                             (const ::sockaddr *)&(_addr.sa()),
                             _addr.inet6() ? sizeof(_addr.sa().v6) : sizeof(_addr.sa().v4));
    if (_size < 0) {
        _ec = last_error();
        return 0;
    } else {
        _ec = std::error_code();
    }
    _buf.rreset(_buf.rindex() + _size);
    return _size;
}

size_t
socket::recvfrom (address &_addr, buffer &_buf, size_t _nbytes, int _flags) KNGIN_EXP
{
    assert(!m_rd_closed);
    check(_buf.writeable(_nbytes));
    socklen_t _addr_len = (_addr.inet6() ? sizeof(_addr.sa().v6) : sizeof(_addr.sa().v4));
    ssize_t _size = ::recvfrom(m_fd, (char *)_buf.data(), _buf.writeable(_nbytes), _flags,
                               (::sockaddr *)&(_addr.sa()),
                               &_addr_len);
    if (_size < 0)
        throw k::system_error("::recvfrom() error");
    _buf.wreset(_buf.windex() + _size);
    return _size;
}

size_t
socket::recvfrom (address &_addr, buffer &_buf, size_t _nbytes, int _flags,
                  std::error_code &_ec) KNGIN_NOEXP
{
    assert(!m_rd_closed);
    check(_buf.writeable());
    socklen_t _addr_len = (_addr.inet6() ? sizeof(_addr.sa().v6) : sizeof(_addr.sa().v4));
    ssize_t _size = ::recvfrom(m_fd, (char *)_buf.data(), _buf.writeable(_nbytes), _flags,
                               (::sockaddr *)&(_addr.sa()),
                               &_addr_len);
    if (_size < 0) {
        _ec = last_error();
        return 0;
    } else {
        _ec = std::error_code();
    }
    _buf.wreset(_buf.windex() + _size);
    return _size;
}

address
socket::localaddr () KNGIN_EXP
{
    assert(!m_wr_closed || !m_rd_closed);
    k::address _addr;
    socklen_t _len = sizeof(_addr.m_sa);
    if (::getsockname(m_fd, (::sockaddr *)&(_addr.m_sa), &_len) < 0)
        throw k::system_error("::getsockname() error");
    return _addr;
}

address
socket::localaddr (std::error_code &_ec) KNGIN_NOEXP
{
    assert(!m_wr_closed || !m_rd_closed);
    k::address _addr;
    socklen_t _len = sizeof(_addr.m_sa);
    _ec = (::getsockname(m_fd, (::sockaddr *)&(_addr.m_sa), &_len) < 0)
          ? last_error()
          : std::error_code();
    return _addr;
}

address
socket::peeraddr () KNGIN_EXP
{
    assert(!m_wr_closed || !m_rd_closed);
    k::address _addr;
    socklen_t _len = sizeof(_addr.m_sa);
    if (::getpeername(m_fd, (::sockaddr *)&(_addr.m_sa), &_len) < 0)
        throw k::system_error("::getpeername() error");
    return _addr;
}

address
socket::peeraddr (std::error_code &_ec) KNGIN_NOEXP
{
    assert(!m_wr_closed || !m_rd_closed);
    k::address _addr;
    socklen_t _len = sizeof(_addr.m_sa);
    _ec = (::getpeername(m_fd, (::sockaddr *)&(_addr.m_sa), &_len) < 0)
          ? last_error()
          : std::error_code();
    return _addr;
}

KNGIN_NAMESPACE_K_END
