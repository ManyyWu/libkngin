#include "kngin/core/base/exception.h"
#include "kngin/core/base/system_error.h"
#include "kngin/net/address.h"
#if defined(KNGIN_NOT_SYSTEM_WIN32)
#include <arpa/inet.h>
#endif /* defined(KNGIN_NOT_SYSTEM_WIN32) */

namespace k {

address::address (const char *ip, uint16_t port, bool v6) {
  ::memset(&sa_, 0, sizeof(sockaddr_u));
  if (v6) {
    sa_.v6.sin6_port = ::htons(port);
    sa_.v6.sin6_family = AF_INET6;
  } else {
    sa_.v4.sin_port = ::htons(port);
    sa_.v4.sin_family = AF_INET;
  }
  auto ret = ::inet_pton(v6 ? AF_INET6 : AF_INET, ip,
                         v6 ? static_cast<void *>(&sa_.v6.sin6_addr)
                                 : static_cast<void *>(&sa_.v4.sin_addr));
  if (!ret)
    throw_system_error("::invalid inet4(6) address", KNGIN_ERRNO(errno));
}

std::string
address::ip_address () const {
  char buf[INET6_ADDRSTRLEN];
  bool ip6 = ipv6();
  auto *ret = ::inet_ntop(
      ip6 ? AF_INET6 : AF_INET,
      ip6 ? static_cast<const void *>(&sa_.v6.sin6_addr)
          : static_cast<const void *>(&sa_.v4.sin_addr),
      buf, size());
  if (!ret)
    throw_system_error("::inet_ntop() error", KNGIN_ERRNO(errno));
  return ret;
}

bool
address::is_valid_ipv4_string (const char *ip) {
  struct ::sockaddr_in sa;
  return ::inet_pton(AF_INET, ip, &sa);
}

bool
address::is_valid_ipv6_string (const char *ip) {
  struct ::sockaddr_in sa;
  return ::inet_pton(AF_INET6, ip, &sa);
}

} /* namespace k */