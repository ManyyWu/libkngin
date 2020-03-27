#include "kngin/core/base/exception.h"
#include "kngin/core/base/system_error.h"
#include "kngin/net/addres.h"
#if !defined(KNGIN_SYSTEM_WIN32)
#include <arpa/inet.h>
#endif /* !defined(KNGIN_SYSTEM_WIN32) */

KNGIN_NAMESPACE_K_BEGIN

address::address (const std::string &addrstr, uint16_t port, bool v6)
{
  if (v6) {
    sa_.v6.sin6_port = ::htons(port);
    sa_.v6.sin6_family = AF_INET6;
  } else {
    sa_.v4.sin_port = ::htons(port);
    sa_.v4.sin_family = AF_INET;
  }
  auto ret = ::inet_pton(v6 ? AF_INET6 : AF_INET,
                         addrstr.c_str(),
                         v6 ? static_cast<void *>(&sa_.v6.sin6_addr)
                                 : static_cast<void *>(&sa_.v4.sin_addr));
  if (!ret)
    throw_system_error("::inet_pton() error", ERRNO(errno));
}

std::string
address::addrstr () const
{
  std::string str;
  str.reserve(size() + 1);
  bool ip6 = inet6();
  auto *ret = ::inet_ntop(
      ip6 ? AF_INET6 : AF_INET,
      ip6 ? static_cast<const void *>(&sa_.v6.sin6_addr)
              : static_cast<const void *>(&sa_.v4.sin_addr),
      str.data(),
      size() + 1);
  if (!ret)
    throw_system_error("::inet_ntop() error", ERRNO(errno));
  return str;
}

bool
address::is_valid_inet_addrstr (const std::string &addrstr)
{
  struct ::sockaddr_in sa;
  int ret = ::inet_pton(AF_INET, addrstr.data(), &sa);
  if (!ret)
    throw_system_error("::inet_pton() error", ERRNO(errno));
  return ret;
}

bool
address::is_valid_inet6_addrstr (const std::string &addrstr)
{
  struct ::sockaddr_in sa;
  int ret = ::inet_pton(AF_INET6, addrstr.data(), &sa);
  if (!ret)
    throw_system_error("::inet_pton() error", ERRNO(errno));
  return ret;
}

KNGIN_NAMESPACE_K_END