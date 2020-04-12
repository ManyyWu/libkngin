#ifndef KNGIN_TIMERFD_TIMER_H
#define KNGIN_TIMERFD_TIMER_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_TIMERFD_TIMER)

#include "kngin/core/event/timer_id.h"
#include "detail/core/event/impl/epoll_event.h"
#include <memory>

namespace k::detail::impl {

class timerfd_timer
  : public obj_entry<timerfd_timer> {
  class operation;
  friend class detail::timer_queue;
  friend class timerfd_timer::operation;

public:
  timerfd_timer (timeout_handler &&handler,
                 timestamp initval, timestamp interval);

  ~timerfd_timer () noexcept;

  epoll_event &
  event () noexcept {
    return ev_;
  }

  void
  set_time (timestamp initval, timestamp interval);

  void
  close () ;

  bool
  closed () const noexcept {
    return HANDLE_INVALID(handle_);
  }

  bool
  registed () const noexcept {
    return ev_.registed();
  }

  std::shared_ptr<timerfd_timer>
  self () {
    return shared_from_this();
  }

  const timer_id &
  id () const noexcept {
    return id_;
  }

private:
  void
  on_events (event_loop &loop, int events);

private:
  handle_t handle_;

  timeout timeout_;

  timeout_handler handler_;

  timer_id id_;

  epoll_event ev_;
};

} /* namespace k::detail::impl */

#endif /* defined(KNGIN_USE_TIMERFD_TIMER) */

#endif /* KNGIN_TIMERFD_TIMER_H */
