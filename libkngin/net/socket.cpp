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
    if (::closesocket(fd_) < 0) {
        fd_ = filefd::invalid_fd;
        throw k::system_error("::close() error");
    }
    fd_ = filefd::invalid_fd;
}

void
filefd::close (error_code &ec) noexcept
{
    if (invalid())
        return;
    ec = (::closesocket(fd_) < 0) ? last_error() : error_code();
    fd_ = filefd::invalid_fd;
}
#endif

socket::socket (socket_type fd)
    : filefd(fd),
      rd_closed_(invalid()),
      wr_closed_(invalid())
{
    if (invalid())
        throw k::exception("socket::socket() error, invalid file descriptor");
}

socket::socket (INET_PROTOCOL proto)
    try
    : filefd(::socket(is_bits_set(proto, 1) ? AF_INET6 : AF_INET,
                      is_bits_set(proto, 0) ? SOCK_DGRAM : SOCK_STREAM, 0)),
      rd_closed_(invalid()),
      wr_closed_(invalid())
{
    arg_check(valid());
} catch (...) {
    log_error("socket::socket() error");
    throw;
}

socket::socket (socket &&s) noexcept
    : filefd(s.fd_),
      rd_closed_(s.rd_closed_),
      wr_closed_(s.wr_closed_)
{
    s.fd_ = filefd::invalid_fd;
    s.rd_closed_ = true;
    s.wr_closed_ = true;
}

socket::~socket () noexcept
{
    fd_ = (rd_closed_ and wr_closed_) ? filefd::invalid_fd : fd_;
}

void
socket::bind (const address &addr)
{
    assert(!wr_closed_);
    assert(!rd_closed_);
    if (::bind(fd_, (const struct ::sockaddr *)&addr.sa(), addr.size()) < 0)
        throw k::system_error("::bind() error");
}

void
socket::bind (const address &addr, error_code &ec) noexcept
{
    assert(!wr_closed_);
    assert(!rd_closed_);
    ec = (::bind(fd_, (const struct ::sockaddr *)&addr.sa_, addr.size()) < 0)
          ? last_error()
          : error_code();
}

void
socket::listen (int backlog)
{
    assert(!wr_closed_);
    assert(!rd_closed_);
    if (::listen(fd_, backlog) < 0)
        throw k::system_error("::listen() error");
}

void
socket::listen (int backlog, error_code &ec) noexcept
{
    assert(!wr_closed_);
    assert(!rd_closed_);
    ec = (::listen(fd_, backlog) < 0) ? last_error() : error_code();
}

socket_type
socket::accept (address &addr)
{
    assert(!wr_closed_);
    assert(!rd_closed_);
    socklen_t len = sizeof(addr.sa_);
    socket_type fd = ::accept(fd_, (struct ::sockaddr *)&addr.sa_, &len);
    if (fd < 0)
        throw k::system_error("::accept() error");
    return fd;
}

socket_type
socket::accept (address &addr, error_code &ec) noexcept
{
    assert(!wr_closed_);
    assert(!rd_closed_);
    socklen_t len = sizeof(addr.sa_);
    socket_type fd = ::accept(fd_, (struct ::sockaddr *)&addr.sa_, &len);
    ec = (fd < 0) ? last_error() : error_code();
    return fd;
}

void
socket::connect (const address &addr)
{
    assert(!wr_closed_);
    assert(!rd_closed_);
    if (::connect(fd_, (const struct ::sockaddr *)&addr.sa_, addr.size()) < 0)
        throw k::system_error("::connect() error");
}

void
socket::connect (const address &addr, error_code &ec) noexcept
{
    assert(!wr_closed_);
    assert(!rd_closed_);
    ec = (::connect(fd_, (const struct ::sockaddr *)&addr.sa_, addr.size()) < 0)
          ? last_error()
          : error_code();
}

void
socket::close ()
{
    filefd::close();
    rd_closed_ = true;
    wr_closed_ = true;
}

void
socket::close (error_code &ec) noexcept
{
    filefd::close(ec);
    rd_closed_ = true;
    wr_closed_ = true;
}

void
socket::rd_shutdown ()
{
    assert(!rd_closed_);
    if (::shutdown(fd_, SHUT_RD) < 0)
        throw k::system_error("::shutdown(SHUT_RD) error");
    fd_ = ((rd_closed_ = true) and wr_closed_) ? filefd::invalid_fd : fd_;
}

void
socket::rd_shutdown (error_code &ec) noexcept
{
    assert(!rd_closed_);
    ec = (::shutdown(fd_, SHUT_RD) < 0 ? last_error() : error_code());
    fd_ = (rd_closed_ and (wr_closed_ = true)) ? filefd::invalid_fd : fd_;
}

void
socket::wr_shutdown ()
{ 
    assert(!wr_closed_);
    if (::shutdown(fd_, SHUT_WR) < 0)
        throw k::system_error("::shutdown(SHUT_WR) error");
    fd_ = ((rd_closed_ = true) and wr_closed_) ? filefd::invalid_fd : fd_;
}

void
socket::wr_shutdown (error_code &ec) noexcept
{
    assert(!wr_closed_);
    ec = (::shutdown(fd_, SHUT_WR) < 0 ? last_error() : error_code());
    fd_ = (rd_closed_ and (wr_closed_ = true)) ? filefd::invalid_fd : fd_;
}

size_t
socket::send (out_buffer &buf, int flags)
{
    assert(buf.size());
    assert(!wr_closed_);
    auto size = ::send(fd_, (char *)buf.begin(), len_type(buf.size()), flags);
    if (size < 0)
        throw k::system_error("::send() error");
    buf -= size;
    return size;
}

size_t
socket::send (out_buffer &buf, int flags, error_code &ec) noexcept
{
    assert(buf.size());
    assert(!wr_closed_);
    auto size = ::send(fd_, (char *)buf.begin(), len_type(buf.size()), flags);
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
    assert(!rd_closed_);
    auto size = ::recv(fd_, (char *)buf.begin(), len_type(buf.writeable()), flags);
    if (size < 0)
        throw k::system_error("::recv() error");
    buf += size;
    return size;
}

size_t
socket::recv (in_buffer &buf, int flags, error_code &ec) noexcept
{
    assert(buf.writeable());
    assert(!rd_closed_);
    auto size = ::recv(fd_, (char *)buf.begin(), len_type(buf.writeable()), flags);
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
    assert(!wr_closed_);
    auto size = ::sendto(fd_, (const char *)buf.begin(), len_type(buf.size()), flags,
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
    assert(!wr_closed_);
    auto size = ::sendto(fd_, (const char *)buf.begin(), len_type(buf.size()), flags,
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
    assert(!rd_closed_);
    socklen_t addr_len = (addr.inet6() ? sizeof(addr.sa().v6) : sizeof(addr.sa().v4));
    auto size = ::recvfrom(fd_, (char *)buf.begin(), len_type(buf.writeable()), flags,
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
    assert(!rd_closed_);
    socklen_t addr_len = (addr.inet6() ? sizeof(addr.sa().v6) : sizeof(addr.sa().v4));
    auto size = ::recvfrom(fd_, (char *)buf.begin(), len_type(buf.writeable()), flags,
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
    assert(!wr_closed_ or !rd_closed_);
    k::address addr;
    socklen_t len = sizeof(addr.sa_);
    if (::getsockname(fd_, (struct ::sockaddr *)&addr.sa_, &len) < 0)
        throw k::system_error("::getsockname() error");
    return addr;
}

address
socket::localaddr (error_code &ec) const noexcept
{
    assert(!wr_closed_ or !rd_closed_);
    k::address addr;
    socklen_t len = sizeof(addr.sa_);
    ec = (::getsockname(fd_, (struct ::sockaddr *)&addr.sa_, &len) < 0)
          ? last_error()
          : error_code();
    return addr;
}

address
socket::peeraddr () const
{
    assert(!wr_closed_ or !rd_closed_);
    k::address addr;
    socklen_t len = sizeof(addr.sa_);
    if (::getpeername(fd_, (struct ::sockaddr *)&addr.sa_, &len) < 0)
        throw k::system_error("::getpeername() error");
    return addr;
}

address
socket::peeraddr (error_code &ec) const noexcept
{
    assert(!wr_closed_ or !rd_closed_);
    k::address addr;
    socklen_t len = sizeof(addr.sa_);
    ec = (::getpeername(fd_, (struct ::sockaddr *)&addr.sa_, &len) < 0)
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
