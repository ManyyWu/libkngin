#include "kngin/net/socket.h"
#include "kngin/core/base/system_error.h"
#include "detail/core/base/descriptor.h"
#if defined(KNGIN_SYSTEM_WIN32)
#  include "detail/core/base/win_utils.h"
#else
#  include "sys/socket.h"
#endif

namespace k {

socket::socket ()
 : handle_(INVALID_HANDLE),
   flags_(0) {
}

socket::socket (inet_protocol proto)
 : handle_(::socket(proto & flag_ipv6 ? AF_INET6 : AF_INET,
                    proto & flag_udp ? SOCK_DGRAM : SOCK_STREAM,
                    0)),
   flags_(proto) {
  if (HANDLE_INVALID(handle_))
    throw_system_error("::socket() error", last_error());
}

socket::socket (socket &&s) noexcept
 : handle_(INVALID_HANDLE),
   flags_(0) {
  assert(HANDLE_VALID(s.handle_));
  std::swap(handle_, s.handle_);
  std::swap(flags_, s.flags_);
}

socket::~socket () noexcept {
  TRY()
    if (HANDLE_VALID(handle_))
      close();
  IGNORE_EXCP()
}

} /* namespace k */
