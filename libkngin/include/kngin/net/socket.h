#ifndef KNGIN_SOCKET_H
#define KNGIN_SOCKET_H

#include "kngin/core/define.h"
#include "kngin/core/base/buffer.h"
#include "kngin/net/address.h"
#include "kngin/net/detail.h"

KNGIN_NAMESPACE_K_BEGIN

class socket {
public:
  enum inet_protocol {
    IPV4_TCP = 0x0,
    IPV4_UDP = 0x1,
    IPV6_TCP = 0x2,
    IPV6_UDP = 0x3,
  };

  socket ();

  explicit
  socket (handle_t h);

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

  handle_t
  accept (address &addr);

  handle_t
  accept (address &addr, error_code &ec) noexcept;

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
  write (out_buffer &buf);

  size_t
  write (out_buffer &buf, error_code &ec) noexcept;

  size_t
  send (out_buffer &buf, int flags);

  size_t
  send (out_buffer &buf, int flags, error_code &ec) noexcept;

  size_t
  recv (in_buffer &buf, int flags);

  size_t
  recv (in_buffer &buf, int flags, error_code &ec) noexcept;

  size_t
  sendto (const address &addr, out_buffer &buf, int flags);

  size_t
  sendto (const address &addr, out_buffer &buf, int flags, error_code &ec) noexcept;

  size_t
  recvfrom (address &addr, in_buffer &buf, int flags);

  size_t
  recvfrom (address &addr, in_buffer &buf, int flags, error_code &ec) noexcept;

  handle_t
  handle () const noexcept {
    return handle_;
  }

private:
  struct streams {
    detail::stream *rstream;
    detail::stream *ostream;
  };

  handle_t handle_;

  streams *streams_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_SOCKET_H */
