#include "kngin/core/base/scoped_flag.h"
#include "kngin/core/base/common.h"
#include "kngin/core/event/event_loop.h"
#include "kngin/core/event/detail.h"
#include "detail/core/event/timer.h"
#include "detail/core/event/timer_queue.h"
#include "detail/core/event/reactor.h"

KNGIN_NAMESPACE_K_BEGIN

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
    timerq_ = new timer_queue();
  } catch (...) {
    safe_release(reactor_);
    safe_release(timerq_);
  }
}

event_loop::~event_loop () {
  TRY()
    safe_release(reactor_);
    safe_release(timerq_);
  IGNORE()
}

void
event_loop::run (start_handler &&start /* = nullptr */,
                 stop_handler &&stop /* = nullptr */) {
  bool is_throw = false;
  tid_ = thread::tid();
  looping_ = true;

  debug("event_loop is running in thread %" PRIu64, tid_);

  debug("event_loop is stopped in thread %" PRIu64, tid_);
}

void
event_loop::stop () {
}

void
event_loop::wakeup () {
}

void
event_loop::register_event (event_base &e) {
}

void
event_loop::remove_event (event_base &e) {
}

void
event_loop::update_event (event_base &e) {
}

bool
event_loop::registed (event_base &e) {
}

void
event_loop::run_in_loop (task &&t) {
  if (t) {
    mutex::scoped_lock lock(taskq_mutex_);
    taskq_.push_back(t);
  }
  if (looping_ and !in_loop_thread())
    wakeup();
}

timer_id
event_loop::run_after (timestamp delay, timeout_handler &&handler) {
  mutex::scoped_lock lock(timerq_mutex_);
  return timerq_->insert(timestamp::monotonic() + delay, 0, std::move(handler)).id();
}

timer_id
event_loop::run_every (timestamp interval, timeout_handler &&handler) {
  mutex::scoped_lock lock(timerq_mutex_);
  return timerq_->insert(timestamp::monotonic() + interval, interval, std::move(handler)).id();
}

timer_id
event_loop::run_at (timestamp absval, timeout_handler &&handler) {
  mutex::scoped_lock lock(timerq_mutex_);
  return timerq_->insert(absval - timestamp::realtime() + timestamp::monotonic(),
                         0, std::move(handler)).id();
}

timer_id
event_loop::run_until (timestamp absval, timestamp interval,
                       timeout_handler &&handler) {
  mutex::scoped_lock lock(timerq_mutex_);
  return timerq_->insert(absval - timestamp::realtime() + timestamp::monotonic(),
                         interval, std::move(handler)).id();
}

void
event_loop::cancel (const timer_id &id) {
  auto ptr = id.query_timer();
  if (ptr) {
    mutex::scoped_lock lock(timerq_mutex_);
    timerq_->remove(ptr);
  }
}

size_t
event_loop::event_loop::wait () {
  auto delay = 0;
  {
    mutex::scoped_lock lock(timerq_mutex_);
    delay = timerq_->min_delay();
  }

  //reactor_->wait(ready_events_);
}

void
event_loop::process_tasks () {
}

void
event_loop::process_events () {

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
  timerq_->process_ready_timer(ready_timers_, timerq_mutex_);
#endif /* defined(KNGIN_USE_MONOTONIC_TIMER) */
}

void
event_loop::sort_events () {
  // use min heap ?
}

void
event_loop::cancel (timer_ptr &ptr) {
  mutex::scoped_lock lock(timerq_mutex_);
  timerq_->remove(ptr);
}

KNGIN_NAMESPACE_K_END
