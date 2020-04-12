#ifndef KNGIN_EPOLL_REACTOR_H
#define KNGIN_EPOLL_REACTOR_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_EPOLL_REACTOR)

#include "detail/core/event/impl/epoll_event.h"
#include "kngin/core/event/event_loop.h"
#include <vector>

namespace k::detail::impl {

class epoll_reactor {
public:
  typedef std::vector<event_loop::actived_event> event_list;

  epoll_reactor ();

  ~epoll_reactor () noexcept;

  size_t
  wait (event_list &list, timestamp delay);

  void
  wakeup ();

  void
  close ();

  void
  register_event (epoll_event &ev);

  void
  remove_event (epoll_event &ev);

  void
  modify_event (epoll_event &ev);

private:
  void
  update_event (int opt, handle_t h, epoll_event *ev);

  void
  on_wakeup ();

private:
  handle_t epoll_fd_;

  handle_t waker_fd_;
};

} /* namespace k::detail::impl */

#endif /* defined(KNGIN_USE_EPOLL_REACTOR) */

#endif /* KNGIN_EPOLL_REACTOR_H */