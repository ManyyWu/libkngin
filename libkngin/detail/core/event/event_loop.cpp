#include "kngin/core/base/scoped_flag.h"
#include "kngin/core/base/common.h"
#include "kngin/core/event/event_loop.h"
#include "detail/core/event/timer_queue.h"
#include "detail/core/event/op_queue.h"
#include "detail/core/event/reactor.h"

KNGIN_NAMESPACE_K_BEGIN

class event_loop::event_queue : public detail::op_queue {
public:
  typedef operation_base::op_type op_type;

  event_queue ()
   : op_queue() {
  }

  virtual
  ~event_queue () noexcept {
    event_queue::clear();
  }

  virtual
  void
  push (operation_base &op) {
    opq_.push_back(&op);
  }

  virtual
  void
  pop () {
    if (opq_.empty())
      throw_exception("out of range");
    opq_.pop_back();
  }

  virtual
  operation_base &
  top () {
    if (opq_.empty())
      throw_exception("out of range");
    return *opq_.back();
  }

  virtual
  size_t
  size () const noexcept {
    return opq_.size();
  }

  virtual
  bool
  empty () const noexcept {
    return opq_.empty();
  }

  virtual
  void
  clear () {
    opq_.clear();
  }

private:
  typedef std::vector<operation_base *> queue_type;

  queue_type opq_;
};

event_loop::event_loop ()
 : tid_(0),
   reactor_(nullptr),
   looping_(false),
   stop_(false),
   taskq_(),
   taskq_mutex_(),
   timerq_(nullptr),
   ready_timers_(),
   timerq_mutex_(),
   timer_processing_(false),
   stop_barrier_(2) {
  try {
    reactor_ = new reactor();
    timerq_ = new detail::timer_queue(this);
  } catch (...) {
    safe_release(reactor_);
    safe_release(timerq_);
  }
}

event_loop::~event_loop () noexcept {
  TRY()
    stop();
    safe_release(reactor_);
    safe_release(timerq_);
  IGNORE()
}

void
event_loop::run () {
  bool is_throw = false;
  tid_ = thread::tid();
  looping_ = true;
  if (stop_barrier_.destroyed())
    stop_barrier_.reinit(2);

  //debug("event_loop is running in thread %" PRIu64, tid_);

  auto fail = [&] () {
    looping_ = false;
    if (is_throw) {
      if (!stop_barrier_.destroyed())
        stop_barrier_.destroy();
    } else {
      if (!stop_barrier_.destroyed() and stop_barrier_.wait())
        stop_barrier_.destroy();
    }
  };

  try {
    event_queue opq;

    while (!stop_) {
      opq.clear();

      // process queued tasks
      process_tasks();
      if (stop_)
        break;

      // wait for events
      auto size = wait(opq);
      if (stop_)
        break;
      debug("%" PRIu64 " events were captured in event_loop of thread %" PRIu64, size, tid_);

      // sort events
      //sort_events();

      // process events
      process_events(opq);
    }
  } catch (...) {
    is_throw = true;
    fail();
    throw;
  }
  fail();

  //debug("event_loop is stopped in thread %" PRIu64, tid_);
}

void
event_loop::stop () {
  if (looping_) {
    stop_ = true;
    if (in_loop_thread()) {
      if (!stop_barrier_.destroyed())
        stop_barrier_.destroy();
    } else {
      wakeup();
      if (stop_barrier_.wait())
        stop_barrier_.destroy();
    }
    {
      mutex::scoped_lock lock(taskq_mutex_);
      taskq_.clear();
    }
    {
      mutex::scoped_lock lock(timerq_mutex_);
      timerq_->clear();
    }
  }
}

void
event_loop::wakeup () {
  if (looping_ and !in_loop_thread())
    reactor_->wakeup();
}

void
event_loop::register_event (reactor_event &ev) {
  reactor_->register_event(ev);
}

void
event_loop::remove_event (reactor_event &ev) {
  reactor_->remove_event(ev);
}

void
event_loop::update_event (reactor_event &ev) {
  reactor_->modify_event(ev);
}

bool
event_loop::registed (reactor_event &ev) {
  return ev.registed();
}

void
event_loop::run_in_loop (task &&t) {
  if (t) {
    mutex::scoped_lock lock(taskq_mutex_);
    taskq_.push_back(t);
  }
  wakeup();
}

timer_id
event_loop::run_after (timestamp delay, timeout_handler &&handler) {
  timer_id id;
  {
    mutex::scoped_lock lock(timerq_mutex_);
    auto &timer = timerq_->insert(timestamp::monotonic() + delay,
                                  0, std::move(handler));
    reactor_->register_event(timer);
    id = timer.id();
  }

  wakeup();
  return id;
}

timer_id
event_loop::run_every (timestamp interval, timeout_handler &&handler) {
  timer_id id;
  {
    mutex::scoped_lock lock(timerq_mutex_);
    auto &timer = timerq_->insert(timestamp::monotonic() + interval,
                                  interval, std::move(handler));
    reactor_->register_event(timer);
    id = timer.id();
  }
  wakeup();
  return id;
}

timer_id
event_loop::run_at (timestamp realtime, timeout_handler &&handler) {
  timer_id id;
  {
    mutex::scoped_lock lock(timerq_mutex_);
    auto &timer = timerq_->insert(realtime - timestamp::realtime() + timestamp::monotonic(),
                                  0, std::move(handler));
    reactor_->register_event(timer);
    id = timer.id();
  }
  wakeup();
  return id;
}

void
event_loop::cancel (const timer_id &id) {
  auto ptr = id.query_timer();
  if (ptr) {
    {
      mutex::scoped_lock lock(timerq_mutex_);
      reactor_->remove_event(*ptr);
      timerq_->remove(ptr);
    }
    wakeup();
  }
}

size_t
event_loop::event_loop::wait (event_queue &evq) {
  time_t delay = 0;
  {
#if defined(KNGIN_USE_MONOTONIC_TIMER)
    mutex::scoped_lock lock(timerq_mutex_);
    delay = timerq_->empty()
            ? KNGIN_EVENT_LOOP_DEFAULT_DELAY
            : timerq_->min_delay().value();
#elif defined(KNGIN_USE_TIMERFD_TIMER)
    delay = KNGIN_EVENT_LOOP_DEFAULT_DELAY;
#endif
  }
  debug("dealy: %" PRIu64 " ms", delay);
  return reactor_->wait(evq, delay);
}

void
event_loop::process_tasks () {
  taskq_type taskq;
  {
    mutex::scoped_lock lock(taskq_mutex_);
    if (taskq_.size())
      taskq_.swap(taskq);
  }
  for (auto &iter : taskq) {
    TRY()
      iter();
    CATCH_ERROR("event_loop::process_tasks()")
  }
}

void
event_loop::process_events (event_queue &evq) {
  while (evq.size()) {
    TRY()
      if (evq.size())
        evq.top().on_operation(*this);
    CATCH_ERROR("event_loop::process_events()")
    evq.pop();
  }

#if defined(KNGIN_USE_MONOTONIC_TIMER)
  process_timers();
#endif /* defined(KNGIN_USE_MONOTONIC_TIMER) */
}

void
event_loop::process_timers () {
#if defined(KNGIN_USE_MONOTONIC_TIMER)
  {
    mutex::scoped_lock lock(timerq_mutex_);
    timerq_->get_ready_timers(ready_timers_);
  }
  timerq_->process_ready_timers(ready_timers_, timerq_mutex_);
#endif /* defined(KNGIN_USE_MONOTONIC_TIMER) */
}

void
event_loop::cancel (timer_ptr &ptr) {
  mutex::scoped_lock lock(timerq_mutex_);
  reactor_->remove_event(*ptr);
  timerq_->remove(ptr);
}

KNGIN_NAMESPACE_K_END
