#ifndef KNGIN_ADDRESS_H
#define KNGIN_ADDRESS_H

#include "kngin/core/define.h"
#if defined(KNGIN_SYSTEM_WIN32)
#include <ws2ipdef.h>
#else
#include <netinet/in.h>
#endif /* defined(KNGIN_SYSTEM_WIN32) */
#include <cassert>
#include <string>
#include <cstring>

namespace k {

class address {
  friend class socket;

public:
  union in_addr_u {
    struct ::in_addr  v4;
    struct ::in6_addr v6;
  };

  union sockaddr_u {
    struct ::sockaddr_in  v4;
    struct ::sockaddr_in6 v6;
  };

  address () noexcept {
    ::memset(&sa_, 0, sizeof(sa_));
  }

  address (const struct ::sockaddr_in &sa) noexcept {
    assert(AF_INET == sa.sin_family);
    sa_.v4 = sa;
  }

  address (const struct ::sockaddr_in6 &sa) noexcept {
    assert(AF_INET6 == sa.sin6_family);
    sa_.v6 = sa;
  }

  address (const address &sa) noexcept {
    sa_ = sa.sa_;
  }

  address (const char *addrstr, uint16_t port, bool v6);

  ~address () = default;

  bool
  family () const noexcept {
    return reinterpret_cast<const ::sockaddr_in *>(&sa_)->sin_family;
  }

  bool
  ipv4 () const noexcept {
    return (AF_INET == reinterpret_cast<const ::sockaddr_in *>(&sa_)->sin_family);
  }

  bool
  ipv6 () const noexcept {
    return (AF_INET6 == reinterpret_cast<const ::sockaddr_in *>(&sa_)->sin_family);
  }

  size_t
  size () const noexcept {
    return (ipv6() ? sizeof(::sockaddr_in6) : sizeof(::sockaddr_in));
  }

  const sockaddr_u &
  sa () const noexcept {
    return sa_;
  }

  uint16_t
  port () const noexcept {
    return (::ntohs(ipv6() ? sa_.v6.sin6_port : sa_.v4.sin_port));
  }

  std::string
  addrstr () const;

  address &
  operator = (const struct ::sockaddr_in &sa) noexcept {
    assert(AF_INET == sa.sin_family);
    sa_.v4 = sa;
    return *this;
  }

  address &
  operator = (const struct ::sockaddr_in6 &sa) noexcept {
    assert(AF_INET6 == sa.sin6_family);
    sa_.v6 = sa;
    return *this;
  }

  address &
  operator = (const address &sa) noexcept {
    sa_ = sa.sa_;
    return *this;
  }

  bool
  operator == (const address &sa) const noexcept {
    return !::memcmp(&sa_, &sa.sa_, size());
  }

  bool
  operator != (const address &sa) const noexcept {
    return ::memcmp(&sa_, &sa.sa_, size());
  }

  bool
  is_ipv4_mapped () const noexcept {
    return IN6_IS_ADDR_V4MAPPED(&sa_.v6.sin6_addr);
  }

  static
  bool
  is_valid_ipv4_addrstr (const char *addrstr);

  static
  bool
  is_valid_ipv6_addrstr (const char *addrstr);

protected:
  sockaddr_u sa_;
};

} /* namespace k */

#endif /* KNGIN_ADDRESS_H */