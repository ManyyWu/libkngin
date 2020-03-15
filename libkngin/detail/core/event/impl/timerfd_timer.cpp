#include "detail/core/base/descriptor.h"
#include "detail/core/event/impl/timerfd_timer.h"
#if defined(KNGIN_USE_TIMERFD_TIMER)

#include <sys/timerfd.h>

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class timerfd_timer::operation : public operation_base {
  friend class timerfd_timer;
public:

private:
};

timerfd_timer::timerfd_timer (timeout_handler &&handler,
                              timestamp initval, timestamp interval)
 : epoll_event(::timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC)),
   handler_(std::move(handler)),
   id_() {
  if (fd_ < 0)
    throw_system_error("::timerfd_create() error", last_error());
  enable_read();
  set_time(initval, interval);
}

timerfd_timer::~timerfd_timer () noexcept {
  TRY()
    close();
  IGNORE()
}

void
timerfd_timer::set_time (timestamp initval, timestamp interval) {
  itimerspec its;
  initval.to_timespec(its.it_value);
  interval.to_timespec(its.it_interval);
  if (::timerfd_settime(fd_, TFD_TIMER_ABSTIME, &its, nullptr) < 0)
    throw_system_error("::timerfd_settime()", last_error());
}

void
timerfd_timer::close () {
  descriptor::close(fd_);
}

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_TIMERFD_TIMER) */
