#ifndef KNGIN_POSIX_TCP_SESSION_H
#define KNGIN_POSIX_TCP_SESSION_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_POSIX_TCP_SESSION)

#include "kngin/net/detail.h"
#include "kngin/net/tcp/session.h"
#include "kngin/net/service.h"

namespace k::detail::impl {

namespace tcp {

class posix_session {
  friend class istream;
  friend class ostream;

public:
  typedef k::tcp::session::message_handler message_handler;
  typedef k::tcp::session::write_handler write_handler;
  typedef k::tcp::session::oob_handler oob_handler;
  typedef k::tcp::session::close_handler close_handler;
  typedef k::tcp::session::session_list session_list;

  posix_session (service &s, socket &&sock, k::tcp::session &owner,
                 write_handler &&o_cb, message_handler &&i_cb, oob_handler &&oob_cb);

  ~posix_session () noexcept;

  bool
  async_write (const out_buffer &buf, int flags);

  bool
  async_read (in_buffer &buf);

  bool
  async_read_some (in_buffer &buf);

  void
  stop_read ();

  void
  start_read ();

  void
  close (close_handler &&handler);

  void
  shutdown ();

  bool
  closed () const noexcept {
    return (flags_ & flag_closed);
  }

  bool
  is_shutdown () const noexcept {
    return (flags_ & flag_shutdown);
  }

  bool
  is_eof () const noexcept {
    return (flags_ & flag_eof);
  }

  const socket &
  get_socket () const noexcept {
    return socket_;
  }

  const error_code &
  last_error () const noexcept {
    return last_error_;
  }

  event_loop &
  get_loop () noexcept {
    return loop_;
  }

  size_t
  remaining () const noexcept;

  std::string
  ip_address () const;

  uint16_t
  port () const noexcept;

private:
  void
  on_events (event_loop &loop, int events);

private:
  enum {
    flag_closed       = 0x0001,
    flag_shutdown     = 0x0002,
    flag_error        = 0x0004,
    flag_eof          = 0x0008,
  };

  class istream;
  class ostream;

  event_loop &loop_;

  k::tcp::session &owner_;

  socket socket_;

  address addr_;

  istream *istream_;

  ostream *ostream_;

  reactor_event ev_;

  std::atomic_int flags_;

  error_code last_error_;
};

} /* namespace tcp */

} /* namespace k::detail::impl */

#endif /* defined(KNGIN_USE_POSIX_TCP_SESSION) */

#endif /* KNGIN_POSIX_TCP_SESSION_H */
