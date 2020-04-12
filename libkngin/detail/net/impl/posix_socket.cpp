#include "kngin/net/socket.h"
#include "kngin/core/base/system_error.h"
#if defined(KNGIN_SYSTEM_WIN32)
#  include "detail/core/base/win_utils.h"
#else
#  include "sys/socket.h"
#endif

namespace k {

void
socket::bind (const address &addr) {
  assert(!this->closed());
  if (::bind(handle_, reinterpret_cast<const sockaddr *>(&addr.sa()), addr.size()) < 0)
    throw_system_error("::bind() error", last_error());
}

void
socket::bind (const address &addr, error_code &ec) noexcept {
  assert(!this->closed());
  ec = (::bind(handle_, reinterpret_cast<const sockaddr *>(&addr.sa()), addr.size()) < 0)
       ? last_error()
       : error_code();
}

void
socket::listen(int backlog) {
  assert(!this->closed());
  if (::listen(handle_, backlog) < 0)
    throw_system_error("::listen() error", last_error());
}

void
socket::listen(int backlog, error_code &ec) noexcept {
  assert(!this->closed());
  ec = (::listen(handle_, backlog) < 0)
       ? last_error()
       : error_code();
}

void
socket::accept(address &addr, socket &s) {
  assert(!this->closed());
  assert(s.closed());
  socklen_t len = (flags_ & flag_ipv6) ? sizeof(address::sockaddr_u::v6) : sizeof(address::sockaddr_u::v4);
  address::sockaddr_u temp;
  ::memset(&temp, 0, sizeof(temp));
  handle_t handle = ::accept(handle_, reinterpret_cast<sockaddr *>(&temp), &len);
  if (HANDLE_INVALID(handle))
    throw_system_error("::accept() error", last_error());
  if (temp.v4.sin_family != AF_INET && temp.v4.sin_family != AF_INET6)
    throw_system_error("accept return unsupported socket", EPROTO);
  s.handle_ = handle;
  s.flags_ = (flags_ & 0x30);
  if (flags_ & flag_ipv6)
    addr = temp.v6;
  else
    addr = temp.v4;
}

void
socket::accept(address &addr, socket &s, error_code &ec) noexcept {
  assert(!this->closed());
  assert(s.closed());
  socklen_t len = (flags_ & flag_ipv6) ? sizeof(address::sockaddr_u::v6) : sizeof(address::sockaddr_u::v4);
  address::sockaddr_u temp = {{0}};
  s.handle_ = INVALID_HANDLE;
  s.flags_ = 0;
  handle_t handle = ::accept(handle_, reinterpret_cast<sockaddr *>(&temp), &len);
  if (HANDLE_INVALID(handle)) {
    ec = last_error();
  } else {
    if (temp.v4.sin_family != AF_INET && temp.v4.sin_family != AF_INET6) {
      ec = EPROTO;
      return;
    }
    if (flags_ & flag_ipv6)
      addr = temp.v6;
    else
      addr = temp.v4;
    s.handle_ = handle;
    s.flags_ = (flags_ & 0x30);
    ec = error_code();
  }
}

void
socket::connect (const address &addr) {
  assert(!this->closed());
  if (::connect(handle_, reinterpret_cast<const sockaddr *>(&addr.sa()), addr.size()) < 0)
    throw_system_error("::connect() error", last_error());
}

void
socket::connect(const address &addr, error_code &ec) noexcept {
  assert(!this->closed());
  ec = (::connect(handle_, reinterpret_cast<const sockaddr *>(&addr.sa()), addr.size()) < 0)
       ? last_error()
       : error_code();
}

void
socket::close () {
  assert(!this->closed());
  if (HANDLE_VALID(handle_)) {
    flags_ |= flag_shutdown;
    detail::descriptor::close(handle_);
  }
}

void
socket::close (error_code &ec) noexcept {
  assert(!this->closed());
  flags_ |= flag_shutdown;
  detail::descriptor::close(handle_, ec);
}

void
socket::shutdown() {
  assert(!this->closed());
  if (::shutdown(handle_, SHUT_WR) < 0)
    throw_system_error("::shutdown() error", last_error());
  flags_ |= flag_shutdown;
}

void
socket::shutdown(error_code &ec) noexcept {
  assert(!this->closed());
  if (::shutdown(handle_, SHUT_WR) < 0) {
    ec = last_error();
  } else {
    ec = error_code();
    flags_ |= flag_shutdown;
  }
}

size_t
socket::read (in_buffer &buf) {
  assert(!this->closed());
  return detail::descriptor::read(handle_, buf);
}

size_t
socket::read (in_buffer &buf, error_code &ec) noexcept {
//  assert(!this->closed());
  return detail::descriptor::read(handle_, buf, ec);
}

size_t
socket::write (out_buffer buf) {
  assert(!this->closed());
  return detail::descriptor::write(handle_, buf);
}

size_t
socket::write (out_buffer buf, error_code &ec) noexcept {
  assert(!this->closed());
  return detail::descriptor::write(handle_, buf, ec);
}

size_t
socket::recv (in_buffer &buf, int flags) {
  assert(buf.size() ? buf.writeable() : true);
  assert(!this->closed());
  auto size = ::recv(handle_, buf.begin(), buf.writeable(), flags);
  if (size < 0)
    throw_system_error("::recv() error", last_error());
  buf += size;
  return size;
}

size_t
socket::recv (in_buffer &buf, int flags, error_code &ec) noexcept {
  assert(buf.size() ? buf.writeable() : true);
  assert(!this->closed());
  auto size = ::recv(handle_, buf.begin(), buf.writeable(), flags);
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
socket::send (out_buffer buf, int flags) {
  assert(buf.size() ? !buf.eof() : true);
  assert(!this->closed());
  auto size = ::send(handle_, buf.begin(), buf.size(), flags);
  if (size < 0)
    throw_system_error("::write() error", last_error());
  return size;
}

size_t
socket::send (out_buffer buf, int flags, error_code &ec) noexcept {
  assert(buf.size() ? !buf.eof() : true);
  assert(!this->closed());
  auto size = ::send(handle_, buf.begin(), buf.size(), flags);
  if (size < 0) {
    ec = last_error();
    return 0;
  } else {
    ec = error_code();
  }
  return size;
}

size_t
socket::recvfrom (address &addr, in_buffer &buf, int flags) {
  assert(buf.size() ? buf.writeable() : true);
  assert(!this->closed());
  socklen_t len = (flags_ & flag_ipv6) ? sizeof(address::sockaddr_u::v6) : sizeof(address::sockaddr_u::v4);
  auto size = ::recvfrom(handle_, buf.begin(), buf.writeable(), flags,
                         reinterpret_cast<sockaddr *>(&addr), &len);
  if (size < 0)
    throw_system_error("::recvfrom() error", last_error());
  buf += size;
  return size;
}

size_t
socket::recvfrom (address &addr, in_buffer &buf, int flags, error_code &ec) noexcept {
  assert(buf.size() ? buf.writeable() : true);
  assert(!this->closed());
  auto size = ::recv(handle_, buf.begin(), buf.writeable(), flags);
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
socket::sendto (const address &addr, out_buffer buf, int flags) {
  assert(buf.size() ? !buf.eof() : true);
  assert(!this->closed());
  auto size = ::sendto(handle_, buf.begin(), buf.size(), flags,
                       reinterpret_cast<const sockaddr *>(&addr), addr.size());
  if (size < 0)
    throw_system_error("::sendto() error", last_error());
  return size;
}

size_t
socket::sendto (const address &addr, out_buffer buf, int flags, error_code &ec) noexcept {
  assert(buf.size() ? !buf.eof() : true);
  assert(!this->closed());
  auto size = ::sendto(handle_, buf.begin(), buf.size(), flags,
                       reinterpret_cast<const sockaddr *>(&addr), addr.size());
  if (size < 0) {
    ec = last_error();
    return 0;
  } else {
    ec = error_code();
  }
  return size;
}
} /* namespace k */