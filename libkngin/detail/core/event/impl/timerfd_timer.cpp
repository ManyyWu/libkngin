#include "kngin/core/event/event_loop.h"
#include "detail/core/event/op_queue.h"
#include "detail/core/base/descriptor.h"
#include "detail/core/event/impl/timerfd_timer.h"
#if defined(KNGIN_USE_TIMERFD_TIMER)

#include <sys/timerfd.h>

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class timerfd_timer::operation : public operation_base {
public:
  explicit
  operation (timerfd_timer &timer)
   : operation_base(operation_base::op_type::op_read),
     timer_(timer) {
  }

  virtual
  ~operation () noexcept {
  }

  virtual
  void
  on_operation (event_loop &loop) {
    auto self = timer_.self();
    int64_t val = 0;
    in_buffer buf(&val, 8);
    descriptor::read(timer_.fd_, buf);
    if (timer_.handler_) {
      auto now = timestamp::monotonic();
      TRY()
        timer_.handler_(timer_.id_, now);
      CATCH_ERROR("timerfd_timer::on_operation()");
      if (!timer_.timeout_.persist())
        loop.cancel(self);
    }
  }

private:
  timerfd_timer &timer_;
};

class timerfd_timer::timer_op_queue : detail::op_queue {
  friend class timerfd_timer;

public:
  explicit
  timer_op_queue (timerfd_timer &timer)
   : op_queue(),
     op_(timer) {
  }

  virtual
  ~timer_op_queue () = default;

  virtual
  void
  push (operation_base &op) {
  }

  virtual
  void
  pop () {
  }

  virtual
  operation_base &
  top () {
    return op_;
  }

  virtual
  size_t
  size () const noexcept {
    return 1;
  }

  virtual
  bool
  empty () const noexcept {
    return false;
  }

  virtual
  void
  clear () {
  }

private:
  timerfd_timer::operation op_;
};

timerfd_timer::timerfd_timer (timeout_handler &&handler,
                              timestamp initval, timestamp interval)
 try
 : epoll_event(::timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC)),
   timeout_(initval, interval),
   handler_(std::move(handler)),
   id_(),
   opq_(nullptr) {
  opq_ = new timerfd_timer::timer_op_queue(*this);
  if (fd_ < 0)
    throw_system_error("::timerfd_create() error", last_error());
  set_time(initval, interval);
  enable_read();
} catch (...) {
  if (FD_VALID(fd_))
    descriptor::close(fd_);
  safe_release(opq_);
  throw;
}

timerfd_timer::~timerfd_timer () noexcept {
  TRY()
  if (!closed())
    close();
  IGNORE()
  safe_release(opq_);
}

void
timerfd_timer::set_time (timestamp initval, timestamp interval) {
  itimerspec its;
  initval.to_timespec(its.it_value);
  interval.to_timespec(its.it_interval);
  if (::timerfd_settime(fd_, TFD_TIMER_ABSTIME, &its, nullptr) < 0)
    throw_system_error("::timerfd_settime() error", last_error());
  timeout_.reset(initval, interval);
}

void
timerfd_timer::close () {
  if (FD_VALID(fd_))
    descriptor::close(fd_);
}

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_TIMERFD_TIMER) */
