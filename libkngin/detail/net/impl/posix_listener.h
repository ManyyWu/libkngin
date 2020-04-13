#ifndef KNGIN_POSIX_LISTENER_H
#define KNGIN_POSIX_LISTENER_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_POSIX_LISTENER)

#include "kngin/core/event/detail.h"
#include "kngin/net/listener.h"
#include "kngin/net/service.h"
#include "kngin/net/server_opts.h"

namespace k::detail::impl {

class posix_listener {
public:
  typedef listener::session_handler session_handler;

  posix_listener (service &s, const server_opts &opts, session_handler &&handler);

  ~posix_listener () noexcept;

  void
  close ();

  bool
  closed () const noexcept {
    return socket_.closed();
  }

  const address &
  listen_addr () const noexcept {
    return listen_addr_;
  }

private:
  void
  on_events (event_loop &loop, int events);

  void
  on_read (event_loop &loop);

private:
  event_loop &loop_;

  socket socket_;

  address listen_addr_;

  session_handler handler_;

  handle_t idle_file_;

  reactor_event ev_;
};

} /* namespace k::detail::impl */

#endif /* defined(KNGIN_USE_POSIX_LISTENER) */

#endif /* KNGIN_POSIX_LISTENER_H */
