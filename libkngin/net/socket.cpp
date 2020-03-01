#include "core/base/common.h"
#include "core/base/bits.h"
#include "net/socket.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/socket.cpp"

KNGIN_NAMESPACE_K_BEGIN

#ifdef _WIN32
socket_type filefd::invalid_fd = INVALID_SOCKET;

void
filefd::close ()
{
    if (invalid())
        return;
    if (::closesocket(m_fd) < 0) {
        m_fd = filefd::invalid_fd;
        throw k::system_error("::close() error");
    }
    m_fd = filefd::invalid_fd;
}

void
filefd::close (error_code &ec) noexcept
{
    if (invalid())
        return;
    ec = (::closesocket(m_fd) < 0) ? last_error() : error_code();
    m_fd = filefd::invalid_fd;
}
#endif

socket::socket (socket_type fd)
    : filefd(fd),
      m_rd_closed(invalid()),
      m_wr_closed(invalid())
{
    if (invalid())
        throw k::exception("socket::socket() error, invalid file descriptor");
}

socket::socket (INET_PROTOCOL proto)
    try
    : filefd(::socket(is_bits_set(proto, 1) ? AF_INET6 : AF_INET,
                      is_bits_set(proto, 0) ? SOCK_DGRAM : SOCK_STREAM, 0)),
      m_rd_closed(invalid()),
      m_wr_closed(invalid())
{
    arg_check(valid());
} catch (...) {
    log_error("socket::socket() error");
    throw;
}

socket::socket (socket &&s) noexcept
    : filefd(s.m_fd),
      m_rd_closed(s.m_rd_closed),
      m_wr_closed(s.m_wr_closed)
{
    s.m_fd = filefd::invalid_fd;
    s.m_rd_closed = true;
    s.m_wr_closed = true;
}

socket::~socket () noexcept
{
    m_fd = (m_rd_closed and m_wr_closed) ? filefd::invalid_fd : m_fd;
}

void
socket::bind (const address &addr)
{
    assert(!m_wr_closed);
    assert(!m_rd_closed);
    if (::bind(m_fd, (const struct ::sockaddr *)&addr.sa(), addr.size()) < 0)
        throw k::system_error("::bind() error");
}

void
socket::bind (const address &addr, error_code &ec) noexcept
{
    assert(!m_wr_closed);
    assert(!m_rd_closed);
    ec = (::bind(m_fd, (const struct ::sockaddr *)&addr.m_sa, addr.size()) < 0)
          ? last_error()
          : error_code();
}

void
socket::listen (int backlog)
{
    assert(!m_wr_closed);
    assert(!m_rd_closed);
    if (::listen(m_fd, backlog) < 0)
        throw k::system_error("::listen() error");
}

void
socket::listen (int backlog, error_code &ec) noexcept
{
    assert(!m_wr_closed);
    assert(!m_rd_closed);
    ec = (::listen(m_fd, backlog) < 0) ? last_error() : error_code();
}

socket_type
socket::accept (address &addr)
{
    assert(!m_wr_closed);
    assert(!m_rd_closed);
    socklen_t len = sizeof(addr.m_sa);
    socket_type fd = ::accept(m_fd, (struct ::sockaddr *)&addr.m_sa, &len);
    if (fd < 0)
        throw k::system_error("::accept() error");
    return fd;
}

socket_type
socket::accept (address &addr, error_code &ec) noexcept
{
    assert(!m_wr_closed);
    assert(!m_rd_closed);
    socklen_t len = sizeof(addr.m_sa);
    socket_type fd = ::accept(m_fd, (struct ::sockaddr *)&addr.m_sa, &len);
    ec = (fd < 0) ? last_error() : error_code();
    return fd;
}

void
socket::connect (const address &addr)
{
    assert(!m_wr_closed);
    assert(!m_rd_closed);
    if (::connect(m_fd, (const struct ::sockaddr *)&addr.m_sa, addr.size()) < 0)
        throw k::system_error("::connect() error");
}

void
socket::connect (const address &addr, error_code &ec) noexcept
{
    assert(!m_wr_closed);
    assert(!m_rd_closed);
    ec = (::connect(m_fd, (const struct ::sockaddr *)&addr.m_sa, addr.size()) < 0)
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
socket::close (error_code &ec) noexcept
{
    filefd::close(ec);
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
socket::rd_shutdown (error_code &ec) noexcept
{
    assert(!m_rd_closed);
    ec = (::shutdown(m_fd, SHUT_RD) < 0 ? last_error() : error_code());
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
socket::wr_shutdown (error_code &ec) noexcept
{
    assert(!m_wr_closed);
    ec = (::shutdown(m_fd, SHUT_WR) < 0 ? last_error() : error_code());
    m_fd = (m_rd_closed and (m_wr_closed = true)) ? filefd::invalid_fd : m_fd;
}

size_t
socket::send (out_buffer &buf, int flags)
{
    assert(buf.size());
    assert(!m_wr_closed);
    auto size = ::send(m_fd, (char *)buf.begin(), len_type(buf.size()), flags);
    if (size < 0)
        throw k::system_error("::send() error");
    buf -= size;
    return size;
}

size_t
socket::send (out_buffer &buf, int flags, error_code &ec) noexcept
{
    assert(buf.size());
    assert(!m_wr_closed);
    auto size = ::send(m_fd, (char *)buf.begin(), len_type(buf.size()), flags);
    if (size < 0) {
        ec = last_error();
        return 0;
    } else {
        ec = error_code();
    }
    buf -= size;
    return size;
}

size_t
socket::recv (in_buffer &buf, int flags)
{
    assert(buf.writeable());
    assert(!m_rd_closed);
    auto size = ::recv(m_fd, (char *)buf.begin(), len_type(buf.writeable()), flags);
    if (size < 0)
        throw k::system_error("::recv() error");
    buf += size;
    return size;
}

size_t
socket::recv (in_buffer &buf, int flags, error_code &ec) noexcept
{
    assert(buf.writeable());
    assert(!m_rd_closed);
    auto size = ::recv(m_fd, (char *)buf.begin(), len_type(buf.writeable()), flags);
    if (size < 0) {
        ec = last_error();
        return 0;
    } else {
        ec = error_code();
    }
    buf += size;
    return size;
}

size_t
socket::sendto (const address &addr, out_buffer &buf, int flags)
{
    assert(buf.size());
    assert(!m_wr_closed);
    auto size = ::sendto(m_fd, (const char *)buf.begin(), len_type(buf.size()), flags,
                             (const struct ::sockaddr *)&addr.sa(),
                             addr.inet6() ? sizeof(addr.sa().v6) : sizeof(addr.sa().v4));
    if (size < 0)
        throw k::system_error("::sendto() error");
    buf -= size;
    return size;
}

size_t
socket::sendto (const address &addr, out_buffer &buf, int flags,
                error_code &ec) noexcept
{
    assert(buf.size());
    assert(!m_wr_closed);
    auto size = ::sendto(m_fd, (const char *)buf.begin(), len_type(buf.size()), flags,
                             (const struct ::sockaddr *)&addr.sa(),
                             addr.inet6() ? sizeof(addr.sa().v6) : sizeof(addr.sa().v4));
    if (size < 0) {
        ec = last_error();
        return 0;
    } else {
        ec = error_code();
    }
    buf -= size;
    return size;
}

size_t
socket::recvfrom (address &addr, in_buffer &buf, int flags)
{
    assert(buf.writeable());
    assert(!m_rd_closed);
    socklen_t addr_len = (addr.inet6() ? sizeof(addr.sa().v6) : sizeof(addr.sa().v4));
    auto size = ::recvfrom(m_fd, (char *)buf.begin(), len_type(buf.writeable()), flags,
                               (struct ::sockaddr *)&addr.sa(),
                               &addr_len);
    if (size < 0)
        throw k::system_error("::recvfrom() error");
    buf += size;
    return size;
}

size_t
socket::recvfrom (address &addr, in_buffer &buf, int flags,
                  error_code &ec) noexcept
{
    assert(buf.writeable());
    assert(!m_rd_closed);
    socklen_t addr_len = (addr.inet6() ? sizeof(addr.sa().v6) : sizeof(addr.sa().v4));
    auto size = ::recvfrom(m_fd, (char *)buf.begin(), len_type(buf.writeable()), flags,
                               (struct ::sockaddr *)&addr.sa(),
                               &addr_len);
    if (size < 0) {
        ec = last_error();
        return 0;
    } else {
        ec = error_code();
    }
    buf += size;
    return size;
}

address
socket::localaddr () const
{
    assert(!m_wr_closed or !m_rd_closed);
    k::address addr;
    socklen_t len = sizeof(addr.m_sa);
    if (::getsockname(m_fd, (struct ::sockaddr *)&addr.m_sa, &len) < 0)
        throw k::system_error("::getsockname() error");
    return addr;
}

address
socket::localaddr (error_code &ec) const noexcept
{
    assert(!m_wr_closed or !m_rd_closed);
    k::address addr;
    socklen_t len = sizeof(addr.m_sa);
    ec = (::getsockname(m_fd, (struct ::sockaddr *)&addr.m_sa, &len) < 0)
          ? last_error()
          : error_code();
    return addr;
}

address
socket::peeraddr () const
{
    assert(!m_wr_closed or !m_rd_closed);
    k::address addr;
    socklen_t len = sizeof(addr.m_sa);
    if (::getpeername(m_fd, (struct ::sockaddr *)&addr.m_sa, &len) < 0)
        throw k::system_error("::getpeername() error");
    return addr;
}

address
socket::peeraddr (error_code &ec) const noexcept
{
    assert(!m_wr_closed or !m_rd_closed);
    k::address addr;
    socklen_t len = sizeof(addr.m_sa);
    ec = (::getpeername(m_fd, (struct ::sockaddr *)&addr.m_sa, &len) < 0)
          ? last_error()
          : error_code();
    return addr;
}

std::string
socket::name () const
{
    std::string name("[");
    name.reserve(KNGIN_CONN_SHORT_NAME_LEN);

    address peer_addr = peeraddr();
    if (peer_addr.inet6()) {
        name.append("[")
             .append(peer_addr.addrstr())
             .append("]:")
             .append(std::to_string(peer_addr.port()))
             .append("]");
    } else {
        name.append(peer_addr.addrstr())
             .append(":")
             .append(std::to_string(peer_addr.port()))
             .append("]");
    }
    return name;
}

/*
std::string
socket::full_name () const
{
    std::string name("[");
    name.reserve(KNGIN_CONN_LONG_NAME_LEN);

    address local_addr = localaddr();
    address peer_addr = peeraddr();
    if (local_addr.inet6()) {
        if (peer_addr.inet6()) {
            name.append("[").append(peer_addr.addrstr()).append("]:")
                 .append(std::to_string(peer_addr.port())).append("]-")
                 .append("[").append(peer_addr.addrstr()).append("]:")
                 .append(std::to_string(peer_addr.port())).append("]");
        } else {
            name.append(peer_addr.addrstr()).append(":")
                 .append(std::to_string(peer_addr.port())).append("-")
                 .append("[").append(peer_addr.addrstr()).append("]:")
                 .append(std::to_string(peer_addr.port())).append("]");
        }
    } else {
        if (peer_addr.inet6()) {
            name.append(local_addr.addrstr()) .append(":")
                 .append(std::to_string(local_addr.port())) .append("-")
                 .append("[").append(peer_addr.addrstr()).append("]:")
                 .append(std::to_string(peer_addr.port())).append("]");
        } else {
            name.append(local_addr.addrstr()) .append(":")
                 .append(std::to_string(local_addr.port())) .append("-")
                 .append(peer_addr.addrstr()) .append(":")
                 .append(std::to_string(peer_addr.port())) .append("]");
        }
    }
    return name;
}
*/

KNGIN_NAMESPACE_K_END
