#ifndef KNGIN_TIMERFD_TIMER_H
#define KNGIN_TIMERFD_TIMER_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_TIMERFD_TIMER)

#include "kngin/core/event/timer_id.h"
#include "detail/core/event/impl/epoll_event.h"
#include <memory>

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class timerfd_timer
  : public epoll_event,
    public obj_node::entry_base<timerfd_timer> {
  friend class detail::timer_queue;
public:
  class operation;
  typedef std::shared_ptr<obj_node::entry_base<timerfd_timer>> self_type;

  timerfd_timer (timeout_handler &&handler,
                 timestamp initval, timestamp interval);

  virtual
  ~timerfd_timer () noexcept;

  void
  set_time (timestamp initval, timestamp interval);

  void
  close () ;

  bool
  closed () const noexcept {
    return FD_INVALID(fd_);
  }

  self_type
  self () {
    return shared_from_this();
  }

  const timer_id &
  id () const noexcept {
    return id_;
  }

private:
  timeout_handler handler_;

  timer_id id_;
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_TIMERFD_TIMER) */

#endif /* KNGIN_TIMERFD_TIMER_H */
