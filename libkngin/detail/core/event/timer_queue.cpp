#include "kngin/core/base/common.h"
#include "kngin/core/base/scoped_lock.h"
#include "detail/core/event/timer_queue.h"
#include "detail/core/event/timer.h"
#include <memory>

KNGIN_NAMESPACE_K_DETAIL_BEGIN

timer_queue::timer_queue ()
 : timers_(0),
   mutex_() {
}

timer_queue::~timer_queue () {
  timers_.clear();
}

timer &
timer_queue::insert (timestamp now_time, timestamp delay, bool persist,
                     timeout_handler &&handler) {

  auto new_timer = std::make_shared<timer>(std::move(handler));
  timers_.insert(new_timer);
  new_timer->set_time(now_time, delay, persist);
  return *new_timer;
}

void
timer_queue::remove (timer_ptr &timer) {
  return_if(timer);
  timers_.remove(timer);
}

timestamp
timer_queue::process_ready_timer () {
#if defined(KNGIN_USE_MONOTONIC_TIMER)
  timestamp min_delay = timestamp::max;
  auto now_time = timestamp::monotonic();
  {
    mutex::scoped_lock lock(mutex_);
    timers_.for_each([&] (std::shared_ptr<timer> &timer) {
      auto &timeout = timer->get_timeout();
      auto remaining = timeout.remaining(now_time);

      // remove
      if (timer->closed()) {
remove:
        timers_.remove(timer);
        return;
      }

      // handle timer
      if (!remaining)
        return;
      TRY()
        auto &t = *timer;
        t.on_events();
      CATCH_ERROR("timer::handler() errror");

      if (!timer->closed() and !timeout.persist()) {
        goto remove;
      } else {
        timeout.update(now_time);
      }
    });
  }
#endif /* defined(KNGIN_USE_MONOTONIC_TIMER) */
}

KNGIN_NAMESPACE_K_DETAIL_END
