#ifndef KNGIN_EVENT_LOOP_H
#define KNGIN_EVENT_LOOP_H

#include "kngin/core/define.h"
#include "kngin/core/base/thread.h"
#include "kngin/core/base/lock.h"
#include "kngin/core/base/barrier.h"
#include "kngin/core/event/detail.h"
#include "kngin/core/event/timer_id.h"
#include "kngin/core/base/callback.h"
#include <memory>
#include <deque>
#include <atomic>

KNGIN_NAMESPACE_K_BEGIN

class event_loop : public noncopyable {
public:
  typedef std::shared_ptr<timer> timer_ptr;

  event_loop ();

  ~event_loop () noexcept;

  void
  run ();

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
  register_event (reactor_event &ev);

  void
  remove_event (reactor_event &ev);

  void
  update_event (reactor_event &ev);

  bool
  registed (reactor_event &ev);

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

  void
  cancel (const timer_id &id);

  void
  cancel (timer_ptr &ptr);

private:
  class event_queue;

  size_t
  wait (event_queue &evq);

  void
  process_tasks ();

  void
  process_events (event_queue &evq);

  void
  process_timers ();

private:
  typedef std::deque<task> taskq_type;
  typedef std::vector<timer_ptr> timer_list;

  thread::tid_type tid_;

  reactor *reactor_;

  std::atomic_bool looping_;

  std::atomic_bool stop_;

  taskq_type taskq_;

  mutex taskq_mutex_;

  detail::timer_queue *timerq_;

  timer_list ready_timers_;

  mutex timerq_mutex_;

  barrier stop_barrier_;

  std::atomic_bool timer_queue_processing_;

  std::atomic<reactor_event *> processing_event_;

  std::atomic_bool events_processing_;

  event_queue *opq_;

  rmutex event_rmutex_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_EVENT_LOOP_H */