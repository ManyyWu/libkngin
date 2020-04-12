#include "kngin/core/base/scoped_flag.h"
#include "kngin/core/base/common.h"
#include "kngin/core/event/event_loop.h"
#include "detail/core/event/timer_queue.h"
#include "detail/core/event/reactor.h"
#include <vector>
#include <algorithm>

namespace k {

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
   stop_barrier_(2),
   timer_queue_processing_(false),
   events_processing_(0),
   event_rmutex_() {
  try {
    reactor_ = new reactor();
    timerq_ = new detail::timer_queue(this);
  } catch (...) {
    safe_release(reactor_);
    safe_release(timerq_);
    throw;
  }
}

event_loop::~event_loop () noexcept {
  TRY()
    stop();
    safe_release(reactor_);
    safe_release(timerq_);
  IGNORE_EXCP()
}

void
event_loop::run () {
  bool is_throw = true;
  tid_ = thread::tid();
  looping_ = true;
  if (stop_barrier_.destroyed())
    stop_barrier_.reinit(2);

  log_debug("event_loop is running in thread %" PRIu64, tid_);

  auto fail = [&] () {
    looping_ = false;
    if (is_throw) {
      if (!stop_barrier_.destroyed())
        stop_barrier_.destroy();
    } else {
      if (stop_barrier_.wait())
        stop_barrier_.destroy();
    }
  };

  try {
    while (!stop_) {
      actived_events_.clear();

      // process queued tasks
      process_tasks();
      if (stop_)
        break;

      // wait for events
      auto size = wait();
      if (stop_)
        break;
      log_debug("%" PRIu64 " events were captured in event_loop of thread %" PRIu64, size, tid_);

      // sort events
      //sort_events();

      // process events
      process_events();
    }
    is_throw = false;
  } catch (const k::exception &e) {
    log_fatal("posix_thread::start(), thread = %" PRIu64 ", message = %s",
          thread::tid(), e.what());
    log_fatal("%s", e.dump());
  } catch (const std::exception &e) {
    log_fatal("posix_thread::start(), thread = %" PRIu64 ", message = %s",
          thread::tid(), e.what());
  } catch (...) {
    log_fatal("posix_thread::start(), thread = %" PRIu64 ", message = unkown exception",
          thread::tid());
  }
  fail();

  log_debug("event_loop stopped in thread %" PRIu64, tid_);
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
  if (reactor_ and looping_ and !in_loop_thread())
    reactor_->wakeup();
}

void
event_loop::register_event (reactor_event &ev) {
  if (reactor_)
    reactor_->register_event(ev);
}

void
event_loop::remove_event (reactor_event &ev) {
  if (reactor_) {
    if (events_processing_.load()) {
      rmutex::scoped_lock lock(event_rmutex_);
      auto size = actived_events_.size();
      for (auto i = events_processing_.load(); i > size; ++i)
        if (&ev == actived_events_[i].ev)
          actived_events_[i].ev = nullptr;
    }
    reactor_->remove_event(ev);
  }
}

void
event_loop::update_event (reactor_event &ev) {
#if defined(KNGIN_USE_IOCP_REACTOR)
#else
  if (reactor_)
    reactor_->modify_event(ev);
#endif
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
  timer *new_timer;
  {
    mutex::scoped_lock lock(timerq_mutex_);
    new_timer = &timerq_->insert(timestamp::monotonic() + delay,
                                 0, std::move(handler));
  }
  assert(new_timer);
#if defined(KNGIN_USE_MONOTONIC_TIMER)
  wakeup();
#else
  reactor_->register_event(new_timer->event());
#endif /* defined(KNGIN_USE_MONOTONIC_TIMER) */
  return new_timer->id();
}

timer_id
event_loop::run_every (timestamp interval, timeout_handler &&handler) {
  timer *new_timer;
  {
    mutex::scoped_lock lock(timerq_mutex_);
    new_timer = &timerq_->insert(timestamp::monotonic() + interval,
                                 interval, std::move(handler));
  }
  assert(new_timer);
#if defined(KNGIN_USE_MONOTONIC_TIMER)
  wakeup();
#else
  reactor_->register_event(new_timer->event());
#endif /* defined(KNGIN_USE_MONOTONIC_TIMER) */
  return new_timer->id();
}

timer_id
event_loop::run_at (timestamp realtime, timeout_handler &&handler) {
  timer *new_timer;
  {
    mutex::scoped_lock lock(timerq_mutex_);
    new_timer = &timerq_->insert(realtime - timestamp::realtime() + timestamp::monotonic(),
                                 0, std::move(handler));
  }
  assert(new_timer);
#if defined(KNGIN_USE_MONOTONIC_TIMER)
  wakeup();
#else
  reactor_->register_event(new_timer->event());
#endif /* defined(KNGIN_USE_MONOTONIC_TIMER) */
  return new_timer->id();
}

void
event_loop::cancel (const timer_id &id) {
  auto ptr = id.query_timer();
#if defined(KNGIN_USE_MONOTONIC_TIMER)
  if (ptr) {
#else
  if (ptr and ptr->registed()) {
    reactor_->remove_event(ptr->event());
#endif /* defined(KNGIN_USE_MONOTONIC_TIMER) */
    {
      mutex::scoped_lock lock(timerq_mutex_);
      timerq_->remove(ptr);
    }
#if defined(KNGIN_USE_MONOTONIC_TIMER)
    wakeup();
#endif /* defined(KNGIN_USE_MONOTONIC_TIMER) */
  }
}

void
event_loop::cancel (timer_ptr &ptr) {
#if defined(KNGIN_USE_MONOTONIC_TIMER)
  if (ptr) {
#else
  if (ptr and ptr->registed()) {
    reactor_->remove_event(ptr->event());
#endif /* defined(KNGIN_USE_MONOTONIC_TIMER) */
    {
      mutex::scoped_lock lock(timerq_mutex_);
      timerq_->remove(ptr);
    }
#if defined(KNGIN_USE_MONOTONIC_TIMER)
    wakeup();
#endif /* defined(KNGIN_USE_MONOTONIC_TIMER) */
  }
}

size_t
event_loop::event_loop::wait () {
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
  log_debug("dealy: %" PRIu64 " ms", delay);
  return reactor_->wait(actived_events_, delay);
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
event_loop::process_events () {
  {
    events_processing_ = true;
    scoped_flag<std::atomic_size_t, size_t> flag(events_processing_, 0);
    rmutex::scoped_lock lock(event_rmutex_);
    for (auto iter : actived_events_) {
      if (iter.ev)
        iter.ev->on_events(*this, iter.events);
    }
  }

#if defined(KNGIN_USE_MONOTONIC_TIMER)
  {
    timer_queue_processing_ = true;
    scoped_flag<std::atomic_bool, bool> flag(timer_queue_processing_, false);
    process_timers();
  }
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

} /* namespace k */
