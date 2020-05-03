#include "kngin/core/define.h"
#if defined(KNGIN_USE_TIMERFD_TIMER)

#include "kngin/core/event/event_loop.h"
#include "detail/core/base/descriptor.h"
#include "detail/core/event/impl/timerfd_timer.h"

#include <sys/timerfd.h>

namespace k::detail::impl {

timerfd_timer::timerfd_timer (timeout_handler &&handler,
                              timestamp initval, timestamp interval)
 try
 : handle_(::timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC)),
   timeout_(initval, interval),
   handler_(std::move(handler)),
   id_(),
   ev_(handle_, [this] (event_loop &loop, int events) {
                  on_events(loop, events);
                }
   ) {
  if (handle_ < 0)
    throw_system_error("::timerfd_create() error", last_error());
  set_time(initval, interval);
  descriptor::set_nonblock(handle_,true);
  descriptor::set_closeexec(handle_, true);
  ev_.enable_read();
  ev_.enable_et();
} catch (...) {
  if (HANDLE_VALID(handle_))
    descriptor::close(handle_);
  throw;
}

timerfd_timer::~timerfd_timer () noexcept {
  TRY()
    close();
  IGNORE_EXCP("timerfd_timer::~timerfd_timer")
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
  do {
    in_buffer buf(&val, 8);
    error_code ec;
    descriptor::read(handle_, buf, ec);
    if (KNGIN_EINTR == ec)
      continue;
    if (KNGIN_EAGAIN == ec)
      break;
    if (ec)
      throw_system_error("descriptor::read() error", ec);
    if (handler_ and events & reactor_event::event_type_read) {
      auto now = timestamp::monotonic();
      TRY()
        handler_(id_, now);
      IGNORE_EXCP("timerfd_timer::on_events");
      if (!timeout_.persist()) {
        loop.cancel(s);
        break;
      }
    }
  } while (true);
}

} /* namespace k::detail::impl */

#endif /* defined(KNGIN_USE_TIMERFD_TIMER) */
