#include "kngin/core/define.h"
#if defined(KNGIN_USE_TIMERFD_TIMER)

#include "kngin/core/event/event_loop.h"
#include "detail/core/base/descriptor.h"
#include "detail/core/event/impl/timerfd_timer.h"

#include <sys/timerfd.h>

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

timerfd_timer::timerfd_timer (timeout_handler &&handler,
                              timestamp initval, timestamp interval)
 try
 : handle_(::timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC)),
   timeout_(initval, interval),
   handler_(std::move(handler)),
   id_(),
   ev_(handle_, std::bind(&timerfd_timer::on_events,
                        this,
                              std::placeholders::_1,
                              std::placeholders::_2
                              )
       ) {
  if (handle_ < 0)
    throw_system_error("::timerfd_create() error", last_error());
  set_time(initval, interval);
  ev_.enable_read();
} catch (...) {
  if (HANDLE_VALID(handle_))
    descriptor::close(handle_);
  throw;
}

timerfd_timer::~timerfd_timer () noexcept {
  TRY()
    close();
  IGNORE_EXCP()
}

void
timerfd_timer::set_time (timestamp initval, timestamp interval) {
  itimerspec its;
  initval.to_timespec(its.it_value);
  interval.to_timespec(its.it_interval);
  if (::timerfd_settime(handle_, TFD_TIMER_ABSTIME, &its, nullptr) < 0)
    throw_system_error("::timerfd_settime() error", last_error());
  timeout_.reset(initval, interval);
}

void
timerfd_timer::close () {
  if (HANDLE_VALID(handle_)) {
    descriptor::close(handle_);
  }
}

void
timerfd_timer::on_events (event_loop &loop, int events) {
  auto s = self();
  int64_t val = 0;
  in_buffer buf(&val, 8);
  descriptor::read(handle_, buf);
  if (handler_ && events & epoll_event::event_type_read) {
    auto now = timestamp::monotonic();
    TRY()
      handler_(id_, now);
    CATCH_ERROR("timerfd_timer::on_events()");
    if (!timeout_.persist())
      loop.cancel(s);
  }
}

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_TIMERFD_TIMER) */
