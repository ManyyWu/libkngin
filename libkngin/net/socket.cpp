#include "core/base/common.h"
#include "core/base/bits.h"
#include "core/base/system_error.h"
#include "net/socket.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/socket.cpp"

KNGIN_NAMESPACE_K_BEGIN

socket::socket (int _fd)
    : filefd(_fd),
      m_rd_closed(invalid()),
      m_wr_closed(invalid())
{
    if (invalid())
        throw k::exception("socket::socket() error, invalid file descriptor");
}

socket::socket (INET_PROTOCOL _proto)
    try
    : filefd(::socket(is_bits_set(_proto, 1) ? AF_INET6 : AF_INET,
                      is_bits_set(_proto, 0) ? SOCK_DGRAM : SOCK_STREAM, 0)),
      m_rd_closed(invalid()),
      m_wr_closed(invalid())
{
    arg_check(valid());
} catch (...) {
    log_error("socket::socket() error");
    throw;
}

socket::socket (socket &&_s) KNGIN_NOEXCP
    : filefd(_s.m_fd),
      m_rd_closed(_s.m_rd_closed),
      m_wr_closed(_s.m_wr_closed)
{
    _s.m_fd = filefd::invalid_fd;
    _s.m_rd_closed = true;
    _s.m_wr_closed = true;
}

socket::~socket () KNGIN_NOEXCP
{
    m_fd = (m_rd_closed and m_wr_closed) ? filefd::invalid_fd : m_fd;
}

void
socket::bind (const address &_addr)
{
    assert(!m_wr_closed);
    assert(!m_rd_closed);
    if (::bind(m_fd, (const struct ::sockaddr *)&_addr.sa(), _addr.size()) < 0)
        throw k::system_error("::bind() error");
}

void
socket::bind (const address &_addr, error_code &_ec) KNGIN_NOEXCP
{
    assert(!m_wr_closed);
    assert(!m_rd_closed);
    _ec = (::bind(m_fd, (const struct ::sockaddr *)&_addr.m_sa, _addr.size()) < 0)
          ? last_error()
          : error_code();
}

void
socket::listen (int _backlog)
{
    assert(!m_wr_closed);
    assert(!m_rd_closed);
    if (::listen(m_fd, _backlog) < 0)
        throw k::system_error("::listen() error");
}

void
socket::listen (int _backlog, error_code &_ec) KNGIN_NOEXCP
{
    assert(!m_wr_closed);
    assert(!m_rd_closed);
    _ec = (::listen(m_fd, _backlog) < 0) ? last_error() : error_code();
}

int
socket::accept (address &_addr)
{
    assert(!m_wr_closed);
    assert(!m_rd_closed);
    socklen_t _len = sizeof(_addr.m_sa);
    int _fd = ::accept(m_fd, (struct ::sockaddr *)&_addr.m_sa, &_len);
    if (_fd < 0)
        throw k::system_error("::accept() error");
    return _fd;
}

int
socket::accept (address &_addr, error_code &_ec) KNGIN_NOEXCP
{
    assert(!m_wr_closed);
    assert(!m_rd_closed);
    socklen_t _len = sizeof(_addr.m_sa);
    int _fd = ::accept(m_fd, (struct ::sockaddr *)&_addr.m_sa, &_len);
    _ec = (_fd < 0) ? last_error() : error_code();
    return _fd;
}

void
socket::connect (const address &_addr)
{
    assert(!m_wr_closed);
    assert(!m_rd_closed);
    if (::connect(m_fd, (const struct ::sockaddr *)&_addr.m_sa, _addr.size()) < 0)
        throw k::system_error("::connect() error");
}

void
socket::connect (const address &_addr, error_code &_ec) KNGIN_NOEXCP
{
    assert(!m_wr_closed);
    assert(!m_rd_closed);
    _ec = (::connect(m_fd, (const struct ::sockaddr *)&_addr.m_sa, _addr.size()) < 0)
          ? last_error()
          : error_code();
}

void
socket::close ()
{
    filefd::close();
    m_rd_closed = true;
    m_wr_closed = true;
}

void
socket::close (error_code &_ec) KNGIN_NOEXCP
{
    filefd::close(_ec);
    m_rd_closed = true;
    m_wr_closed = true;
}

void
socket::rd_shutdown ()
{
    assert(!m_rd_closed);
    if (::shutdown(m_fd, SHUT_RD) < 0)
        throw k::system_error("::shutdown(SHUT_RD) error");
    m_fd = ((m_rd_closed = true) and m_wr_closed) ? filefd::invalid_fd : m_fd;
}

void
socket::rd_shutdown (error_code &_ec) KNGIN_NOEXCP
{
    assert(!m_rd_closed);
    _ec = (::shutdown(m_fd, SHUT_RD) < 0 ? last_error() : error_code());
    m_fd = (m_rd_closed and (m_wr_closed = true)) ? filefd::invalid_fd : m_fd;
}

void
socket::wr_shutdown ()
{ 
    assert(!m_wr_closed);
    if (::shutdown(m_fd, SHUT_WR) < 0)
        throw k::system_error("::shutdown(SHUT_WR) error");
    m_fd = ((m_rd_closed = true) and m_wr_closed) ? filefd::invalid_fd : m_fd;
}

void
socket::wr_shutdown (error_code &_ec) KNGIN_NOEXCP
{
    assert(!m_wr_closed);
    _ec = (::shutdown(m_fd, SHUT_WR) < 0 ? last_error() : error_code());
    m_fd = (m_rd_closed and (m_wr_closed = true)) ? filefd::invalid_fd : m_fd;
}

size_t
socket::send (out_buffer &_buf, int _flags)
{
    assert(_buf.size());
    assert(!m_wr_closed);
    ssize_t _size = ::send(m_fd, _buf.begin(), _buf.size(), _flags);
    if (_size < 0)
        throw k::system_error("::send() error");
    _buf -= _size;
    return _size;
}

size_t
socket::send (out_buffer &_buf, int _flags, error_code &_ec) KNGIN_NOEXCP
{
    assert(_buf.size());
    assert(!m_wr_closed);
    ssize_t _size = ::send(m_fd, _buf.begin(), _buf.size(), _flags);
    if (_size < 0) {
        _ec = last_error();
        return 0;
    } else {
        _ec = error_code();
    }
    _buf -= _size;
    return _size;
}

size_t
socket::recv (in_buffer &_buf, int _flags)
{
    assert(_buf.writeable());
    assert(!m_rd_closed);
    ssize_t _size = ::recv(m_fd, _buf.begin(), _buf.writeable(), _flags);
    if (_size < 0)
        throw k::system_error("::recv() error");
    _buf += _size;
    return _size;
}

size_t
socket::recv (in_buffer &_buf, int _flags, error_code &_ec) KNGIN_NOEXCP
{
    assert(_buf.writeable());
    assert(!m_rd_closed);
    ssize_t _size = ::recv(m_fd, _buf.begin(), _buf.writeable(), _flags);
    if (_size < 0) {
        _ec = last_error();
        return 0;
    } else {
        _ec = error_code();
    }
    _buf += _size;
    return _size;
}

size_t
socket::sendto (const address &_addr, out_buffer &_buf, int _flags)
{
    assert(_buf.size());
    assert(!m_wr_closed);
    ssize_t _size = ::sendto(m_fd, (const char *)_buf.begin(), _buf.size(), _flags,
                             (const struct ::sockaddr *)&_addr.sa(),
                             _addr.inet6() ? sizeof(_addr.sa().v6) : sizeof(_addr.sa().v4));
    if (_size < 0)
        throw k::system_error("::sendto() error");
    _buf -= _size;
    return _size;
}

size_t
socket::sendto (const address &_addr, out_buffer &_buf, int _flags,
                error_code &_ec) KNGIN_NOEXCP
{
    assert(_buf.size());
    assert(!m_wr_closed);
    ssize_t _size = ::sendto(m_fd, (const char *)_buf.begin(), _buf.size(), _flags,
                             (const struct ::sockaddr *)&_addr.sa(),
                             _addr.inet6() ? sizeof(_addr.sa().v6) : sizeof(_addr.sa().v4));
    if (_size < 0) {
        _ec = last_error();
        return 0;
    } else {
        _ec = error_code();
    }
    _buf -= _size;
    return _size;
}

size_t
socket::recvfrom (address &_addr, in_buffer &_buf, int _flags)
{
    assert(_buf.writeable());
    assert(!m_rd_closed);
    socklen_t _addr_len = (_addr.inet6() ? sizeof(_addr.sa().v6) : sizeof(_addr.sa().v4));
    ssize_t _size = ::recvfrom(m_fd, (char *)_buf.begin(), _buf.writeable(), _flags,
                               (struct ::sockaddr *)&_addr.sa(),
                               &_addr_len);
    if (_size < 0)
        throw k::system_error("::recvfrom() error");
    _buf += _size;
    return _size;
}

size_t
socket::recvfrom (address &_addr, in_buffer &_buf, int _flags,
                  error_code &_ec) KNGIN_NOEXCP
{
    assert(_buf.writeable());
    assert(!m_rd_closed);
    socklen_t _addr_len = (_addr.inet6() ? sizeof(_addr.sa().v6) : sizeof(_addr.sa().v4));
    ssize_t _size = ::recvfrom(m_fd, (char *)_buf.begin(), _buf.writeable(), _flags,
                               (struct ::sockaddr *)&_addr.sa(),
                               &_addr_len);
    if (_size < 0) {
        _ec = last_error();
        return 0;
    } else {
        _ec = error_code();
    }
    _buf += _size;
    return _size;
}

address
socket::localaddr () const
{
    assert(!m_wr_closed or !m_rd_closed);
    k::address _addr;
    socklen_t _len = sizeof(_addr.m_sa);
    if (::getsockname(m_fd, (struct ::sockaddr *)&_addr.m_sa, &_len) < 0)
        throw k::system_error("::getsockname() error");
    return _addr;
}

address
socket::localaddr (error_code &_ec) const KNGIN_NOEXCP
{
    assert(!m_wr_closed or !m_rd_closed);
    k::address _addr;
    socklen_t _len = sizeof(_addr.m_sa);
    _ec = (::getsockname(m_fd, (struct ::sockaddr *)&_addr.m_sa, &_len) < 0)
          ? last_error()
          : error_code();
    return _addr;
}

address
socket::peeraddr () const
{
    assert(!m_wr_closed or !m_rd_closed);
    k::address _addr;
    socklen_t _len = sizeof(_addr.m_sa);
    if (::getpeername(m_fd, (struct ::sockaddr *)&_addr.m_sa, &_len) < 0)
        throw k::system_error("::getpeername() error");
    return _addr;
}

address
socket::peeraddr (error_code &_ec) const KNGIN_NOEXCP
{
    assert(!m_wr_closed or !m_rd_closed);
    k::address _addr;
    socklen_t _len = sizeof(_addr.m_sa);
    _ec = (::getpeername(m_fd, (struct ::sockaddr *)&_addr.m_sa, &_len) < 0)
          ? last_error()
          : error_code();
    return _addr;
}

std::string
socket::name () const
{
    std::string _name("[");
    _name.reserve(KNGIN_CONN_SHORT_NAME_LEN);

    address _peer_addr = peeraddr();
    if (_peer_addr.inet6()) {
        _name.append("[")
             .append(_peer_addr.addrstr())
             .append("]:")
             .append(std::to_string(_peer_addr.port()))
             .append("]");
    } else {
        _name.append(_peer_addr.addrstr())
             .append(":")
             .append(std::to_string(_peer_addr.port()))
             .append("]");
    }
    return _name;
}

/*
std::string
socket::full_name () const
{
    std::string _name("[");
    _name.reserve(KNGIN_CONN_LONG_NAME_LEN);

    address _local_addr = localaddr();
    address _peer_addr = peeraddr();
    if (_local_addr.inet6()) {
        if (_peer_addr.inet6()) {
            _name.append("[").append(_peer_addr.addrstr()).append("]:")
                 .append(std::to_string(_peer_addr.port())).append("]-")
                 .append("[").append(_peer_addr.addrstr()).append("]:")
                 .append(std::to_string(_peer_addr.port())).append("]");
        } else {
            _name.append(_peer_addr.addrstr()).append(":")
                 .append(std::to_string(_peer_addr.port())).append("-")
                 .append("[").append(_peer_addr.addrstr()).append("]:")
                 .append(std::to_string(_peer_addr.port())).append("]");
        }
    } else {
        if (_peer_addr.inet6()) {
            _name.append(_local_addr.addrstr()) .append(":")
                 .append(std::to_string(_local_addr.port())) .append("-")
                 .append("[").append(_peer_addr.addrstr()).append("]:")
                 .append(std::to_string(_peer_addr.port())).append("]");
        } else {
            _name.append(_local_addr.addrstr()) .append(":")
                 .append(std::to_string(_local_addr.port())) .append("-")
                 .append(_peer_addr.addrstr()) .append(":")
                 .append(std::to_string(_peer_addr.port())) .append("]");
        }
    }
    return _name;
}
*/

KNGIN_NAMESPACE_K_END
