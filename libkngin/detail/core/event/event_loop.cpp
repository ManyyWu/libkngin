#include "kngin/core/base/log.h"
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
}

timer_id
event_loop::run_after (timestamp delay, timeout_handler &&handler) {
}

timer_id
event_loop::run_every (timestamp interval, timeout_handler &&handler) {
}

timer_id
event_loop::run_at (timestamp absval, timeout_handler &&handler) {
}

timer_id
event_loop::run_until (timestamp absval, timestamp interval,
                       timeout_handler &&handler) {
}

void
event_loop::cancel (timer_id &id) {
}

size_t
event_loop::event_loop::wait () {
}

void
event_loop::process_tasks () {
}

void
event_loop::process_events () {
}

void
event_loop::process_timer () {
}

void
event_loop::sort_events () {
}

void
event_loop::get_ready_timer () {
}

void
event_loop::cancel (const timer_ptr &timer) {

}

KNGIN_NAMESPACE_K_END
