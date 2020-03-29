#ifndef KNGIN_TIMERFD_TIMER_H
#define KNGIN_TIMERFD_TIMER_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_TIMERFD_TIMER)

#include "kngin/core/event/timer_id.h"
#include "detail/core/event/op_queue.h"
#include "detail/core/event/impl/epoll_event.h"
#include <memory>

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class timerfd_timer
  : public epoll_event,
    public obj_entry<timerfd_timer> {
  class operation;
  class timer_op_queue;
  friend class detail::timer_queue;
  friend class timerfd_timer::operation;
  friend class timerfd_timer::timer_op_queue;

public:
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
    return HANDLE_INVALID(handle_);
  }

  std::shared_ptr<timerfd_timer>
  self () {
    return shared_from_this();
  }

  const timer_id &
  id () const noexcept {
    return id_;
  }

  virtual
  op_queue *
  get_op_queue (op_type type) noexcept {
    if (op_type::op_read == type or op_type::op_error == type)
      return opq_;
    return nullptr;
  }

private:
  timeout timeout_;

  timeout_handler handler_;

  timer_id id_;

  op_queue *opq_;
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_TIMERFD_TIMER) */

#endif /* KNGIN_TIMERFD_TIMER_H */
