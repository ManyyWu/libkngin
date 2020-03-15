#ifndef KNGIN_EPOLL_REACTOR_H
#define KNGIN_EPOLL_REACTOR_H

#include "kngin/core/define.h"
#if !defined(KNGIN_SYSTEM_WIN32)

#include "detail/core/event/op_queue.h"
#include "detail/core/event/impl/epoll_event.h"
#include <vector>

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class event_loop;
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
  update_event (int opt, int fd, class epoll_event *ev);

  void
  on_wakeup ();

private:
  int epoll_fd_;

  int waker_fd_;
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* !defined(KNGIN_SYSTEM_WIN32) */

#endif /* KNGIN_EPOLL_REACTOR_H */