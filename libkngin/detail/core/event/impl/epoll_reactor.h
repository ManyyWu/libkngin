#ifndef KNGIN_EPOLL_REACTOR_H
#define KNGIN_EPOLL_REACTOR_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_EPOLL_REACTOR)

#include "detail/core/event/op_queue.h"
#include "detail/core/event/impl/epoll_event.h"
#include <vector>

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class epoll_reactor {
public:
  epoll_reactor ();

  ~epoll_reactor () noexcept;

  size_t
  wait (op_queue &ops, timestamp delay);

  void
  wakeup ();

  void
  close ();

  void
  register_event (class epoll_event &ev);

  void
  remove_event (class epoll_event &ev);

  void
  modify_event (class epoll_event &ev);

private:
  void
  update_event (int opt, handle_t h, class epoll_event *ev);

  void
  on_wakeup ();

private:
  handle_t epoll_fd_;

  handle_t waker_fd_;
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_EPOLL_REACTOR) */

#endif /* KNGIN_EPOLL_REACTOR_H */