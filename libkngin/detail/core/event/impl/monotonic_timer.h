#ifndef KNGIN_MONOTONIC_TIMER_H
#define KNGIN_MONOTONIC_TIMER_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_MONOTONIC_TIMER)

#include "kngin/core/event/timer_id.h"
#include <memory>

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class monotonic_timer
  : public noncopyable,
    public std::enable_shared_from_this<monotonic_timer> {
  friend class detail::timer_queue;

public:
  monotonic_timer (timeout_handler &&handler,
                   timestamp initval, timestamp interval) noexcept;

  ~monotonic_timer () noexcept {
  }

  void
  set_time (timestamp initval, timestamp interval) noexcept;

  void
  close () noexcept;

  bool
  closed () const noexcept {
    return closed_;
  }

  timeout &
  get_timeout () noexcept {
    return timeout_;
  }

  std::shared_ptr<monotonic_timer>
  self () {
    return shared_from_this();
  }

  const timer_id &
  id () const noexcept {
    return id_;
  }

public:
  void
  on_events (const timestamp &now);

private:
  timeout timeout_;

  timeout_handler handler_;

  std::atomic_bool closed_;

  timer_id id_;
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_MONOTONIC_TIMER) */

#endif /* KNGIN_MONOTONIC_TIMER_H */