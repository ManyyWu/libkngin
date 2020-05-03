#ifndef KNGIN_TCP_SESSION_H
#define KNGIN_TCP_SESSION_H

#include "kngin/core/define.h"
#include "kngin/core/base/noncopyable.h"
#include "kngin/net/detail.h"
#include <functional>
#include <vector>
#include <string>

namespace k {

class address;
class socket;
class service;

namespace tcp {

class session : noncopyable {
public:
  typedef std::function<void(session &, const in_buffer &, const error_code &)> message_handler;
  typedef std::function<void(session &, size_t, const error_code &)> write_handler;
  typedef std::function<void(session &, uint8_t, const error_code &)> oob_handler;
  typedef std::function<void(session &)> close_handler;
  typedef std::vector<out_buffer> out_buffers;
  typedef std::vector<session *> session_list;

  session (service &s, socket &&sock,
           write_handler &&o_cb,
           message_handler &&i_cb,
           oob_handler &&oob_cb);

  ~session () noexcept;

  bool
  async_write (out_buffer buf);

  bool
  async_write (out_buffer buf, int flags);

  size_t
  async_write (const out_buffers &bufs);

  bool
  async_read (in_buffer buf);

  bool
  async_read_some (in_buffer buf);

  void
  stop_read ();

  void
  start_read ();

  void
  close (close_handler &&handler);

  void
  force_shutdown ();

  void
  async_shutdown ();

  bool
  closed () const noexcept;

  bool
  is_shutdown () const noexcept;

  bool
  is_eof () const noexcept;

  const error_code &
  last_error () const noexcept;

  event_loop &
  get_loop () noexcept;

  size_t
  remaining () const noexcept;

//  const socket &
//  get_socket () const noexcept;

  std::string
  ip_address () const;

  uint16_t
  port () const noexcept;

  static
  void
  broadcast (const session_list &, out_buffer buf, int flags);

private:
  session_impl *impl_;
};

} /* namespace tcp */

} /* namespace k */

#endif /* KNGIN_TCP_SESSION_H */
