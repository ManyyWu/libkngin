#ifndef KNGIN_EVENT_LOOP_H
#define KNGIN_EVENT_LOOP_H

#include "kngin/core/define.h"
#include "kngin/core/base/thread.h"
#include "kngin/core/base/mutex.h"
#include "kngin/core/base/barrier.h"
#include "kngin/core/event/detail.h"
#include "kngin/core/event/event_base.h"
#include "kngin/core/event/timer_id.h"
#include <memory>
#include <deque>
#include <atomic>

KNGIN_NAMESPACE_K_BEGIN

class event_loop {
public:
  typedef std::shared_ptr<timer> timer_ptr;
  typedef event_loop_handler start_handler;
  typedef event_loop_handler stop_handler;

  event_loop ();

  ~event_loop () noexcept;

  void
  run (start_handler &&start = nullptr, stop_handler &&stop = nullptr);

  void
  stop ();

  void
  wakeup ();

  bool
  looping () const noexcept {
    return looping_;
  }

  bool
  in_loop_thread () const noexcept {
    return (tid_ == thread::tid());
  }

  // event
  void
  register_event (event_base &e);

  void
  remove_event (event_base &e);

  void
  update_event (event_base &e);

  bool
  registed (event_base &e);

  // task
  void
  run_in_loop (task &&t);

  // timer
  timer_id
  run_after (timestamp delay, timeout_handler &&handler);

  timer_id
  run_every (timestamp interval, timeout_handler &&handler);

  timer_id
  run_at (timestamp realtime, timeout_handler &&handler);

  timer_id
  run_until (timestamp realtime, timestamp interval, timeout_handler &&handler);

  void
  cancel (const timer_id &id);

private:
  size_t
  wait ();

  void
  process_tasks ();

  void
  process_events ();

  typedef std::vector<timer_ptr> timer_list;

  void
  process_timers ();

  void
  sort_events ();

  void
  cancel (timer_ptr &ptr);

private:
  typedef std::deque<task> taskq;

  thread::tid_type tid_;

  reactor *reactor_;

  std::atomic_bool looping_;

  std::atomic_bool stop_;

  taskq taskq_;

  mutex taskq_mutex_;

  timer_queue *timerq_;

  timer_list ready_timers_;

  mutex timerq_mutex_;

  std::atomic_bool timer_processing_;

  barrier stop_barrier_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_EVENT_LOOP_H */