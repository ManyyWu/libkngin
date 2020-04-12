#ifndef KNGIN_SOCKET_H
#define KNGIN_SOCKET_H

#include "kngin/core/define.h"
#include "kngin/core/base/buffer.h"
#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/error_code.h"
#include "kngin/net/address.h"

namespace k {

class socket : public noncopyable {
public:
  typedef enum {
    ipv4_tcp = 0x00,
    ipv4_udp = 0x10,
    ipv6_tcp = 0x20,
    ipv6_udp = 0x30,
  } inet_protocol;

  socket ();

  explicit
  socket (inet_protocol proto);

  socket (socket &&s) noexcept;

  ~socket () noexcept;

  void
  bind (const address &addr);

  void
  bind (const address &addr, error_code &ec) noexcept;

  void
  listen (int backlog);

  void
  listen (int backlog, error_code &ec) noexcept;

  void
  accept (address &addr, socket &s);

  void
  accept (address &addr, socket &s, error_code &ec) noexcept;

  void
  connect (const address &addr);

  void
  connect (const address &addr, error_code &ec) noexcept;

  void
  close ();

  void
  close (error_code &ec) noexcept;

  void
  shutdown ();

  void
  shutdown (error_code &ec) noexcept;

  size_t
  read (in_buffer &buf);

  size_t
  read (in_buffer &buf, error_code &ec) noexcept;

  size_t
  write (out_buffer buf);

  size_t
  write (out_buffer buf, error_code &ec) noexcept;

  size_t
  recv (in_buffer &buf, int flags);

  size_t
  recv (in_buffer &buf, int flags, error_code &ec) noexcept;

  size_t
  send (out_buffer buf, int flags);

  size_t
  send (out_buffer buf, int flags, error_code &ec) noexcept;

  size_t
  recvfrom (address &addr, in_buffer &buf, int flags);

  size_t
  recvfrom (address &addr, in_buffer &buf, int flags, error_code &ec) noexcept;

  size_t
  sendto (const address &addr, out_buffer buf, int flags);

  size_t
  sendto (const address &addr, out_buffer buf, int flags, error_code &ec) noexcept;

  bool
  inet4 () const noexcept {
    return !(flags_ & flag_ipv6);
  }

  bool
  inet6 () const noexcept {
    return (flags_ & flag_ipv6);
  }

  bool
  tcp () const noexcept {
    return !(flags_ & flag_udp);
  }

  bool
  udp () const noexcept {
    return (flags_ & flag_udp);
  }

  handle_t
  handle () const noexcept {
    return handle_;
  }

  bool
  closed () const noexcept {
    return HANDLE_INVALID(handle_);
  }

  bool
  is_shutdown () const noexcept {
    return (flags_ & flag_shutdown);
  }

private:
  enum {
    flag_shutdown = 0x04,
    flag_udp      = 0x10,
    flag_ipv6     = 0x20,
  };

  handle_t handle_;

  int flags_;
};

} /* namespace k */

#endif /* KNGIN_SOCKET_H */
