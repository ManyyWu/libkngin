#include "kngin/core/define.h"
#if defined(KNGIN_USE_TIMERFD_TIMER)

#include "kngin/core/event/event_loop.h"
#include "detail/core/event/op_queue.h"
#include "detail/core/base/descriptor.h"
#include "detail/core/event/impl/timerfd_timer.h"

#include <sys/timerfd.h>

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class timerfd_timer::operation
  : public noncopyable,
    public operation_base {
public:
  explicit
  operation (timerfd_timer &timer)
   : operation_base(timer, operation_base::op_type::op_read) {
  }

  virtual
  ~operation () noexcept {
  }

  virtual
  void
  on_operation (event_loop &loop) {
    timerfd_timer &owner = dynamic_cast<timerfd_timer &>(owner_);
    auto self = owner.self();
    int64_t val = 0;
    in_buffer buf(&val, 8);
    descriptor::read(owner.handle_, buf);
    if (owner.handler_) {
      auto now = timestamp::monotonic();
      TRY()
        owner.handler_(owner.id_, now);
      CATCH_ERROR("timerfd_timer::on_operation()");
      if (!owner.timeout_.persist())
        loop.cancel(self);
    }
  }
};

class timerfd_timer::timer_op_queue
  : public noncopyable,
    public detail::op_queue {
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
  if (handle_ < 0)
    throw_system_error("::timerfd_create() error", last_error());
  set_time(initval, interval);
  enable_read();
} catch (...) {
  if (HANDLE_VALID(handle_))
    descriptor::close(handle_);
  safe_release(opq_);
  throw;
}

timerfd_timer::~timerfd_timer () noexcept {
  TRY()
  if (!closed())
    close();
  IGNORE_EXCP()
  safe_release(opq_);
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
  if (HANDLE_VALID(handle_))
    descriptor::close(handle_);
}

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_TIMERFD_TIMER) */
