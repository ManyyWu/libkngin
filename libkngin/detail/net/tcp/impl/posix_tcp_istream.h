#ifndef KNGIN_POSIX_TCP_STREAM_H
#define KNGIN_POSIX_TCP_STREAM_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_POSIX_TCP_SESSION)

#include "kngin/core/base/buffer.h"
#include "kngin/core/base/mutex.h"
#include "kngin/net/socket.h"
#include <deque>
#include <atomic>

namespace k::detail::impl {

namespace tcp {

class posix_session;
class posix_session::istream : noncopyable {
  friend class posix_session;

public:
  typedef k::tcp::session::message_handler message_handler;
  typedef k::tcp::session::oob_handler oob_handler;

  struct in_context {
    in_buffer buffer;

    bool need_complete;
  };
  typedef std::deque<in_context> in_ctxq;

  istream (posix_session &session, socket &sock,
           message_handler &&i_cb, oob_handler &&oob_cb);

  ~istream () noexcept;

  void
  async_read (in_buffer &buf);

  void
  async_read_some (in_buffer &buf);

  void
  clear ();

private:
  void
  on_read ();

  void
  on_oob ();

  void
  message_callback (const in_buffer &buf, const error_code &ec);

  void
  on_completion (bool remove);

private:
  posix_session &session_;

  message_handler message_handler_;

  oob_handler oob_handler_;

  in_ctxq ctxq_;

  mutex ctxq_mutex_;

  std::atomic<in_context *> next_ctx_;

  std::atomic_bool complete_;
};

} /* namespace tcp */

} /* namespace k::detail::impl */

#endif /* defined(KNGIN_USE_POSIX_TCP_SESSION) */

#endif /* KNGIN_POSIX_TCP_STREAM_H */
