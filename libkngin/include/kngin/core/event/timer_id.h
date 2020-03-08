#ifndef KNGIN_TIMER_ID_H
#define KNGIN_TIMER_ID_H

#include "kngin/core/define.h"
#include "kngin/core/base/timeout.h"
#include "kngin/core/event/detail.h"

KNGIN_NAMESPACE_K_BEGIN

class timer_id {
public:
  typedef std::shared_ptr<timer> timer_ptr;
  typedef std::weak_ptr<timer> timer_weak_ptr;

  timer_id () = default;

  timer_id (const timer_id &id) noexcept
   : timer_(id.timer_) {
  }

  explicit
  timer_id (timer_ptr &timer) noexcept
   : timer_(timer) {
  }

  ~timer_id () = default;

  timer_id &
  operator = (const timer_id &id) noexcept {
    timer_ = id.timer_;
    return *this;
  }

  bool
  cancelled () const noexcept {
    return timer_.expired();
  }

private:
  timer_weak_ptr timer_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_TIMER_ID_H */