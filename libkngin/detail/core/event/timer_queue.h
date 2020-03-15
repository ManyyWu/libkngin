#ifndef KNGIN_TIMER_QUEUE_H
#define KNGIN_TIMER_QUEUE_H

#include "kngin/core/base/noncopyable.h"
#if defined(KNGIN_USE_TIMERFD_TIMER)
#include "detail/core/base/obj_mgr.h"
#endif /* defined(KNGIN_USE_TIMERFD_TIMER) */
#include "detail/core/event/timer.h"
#include <vector>
#include <queue>

class event_loop;

KNGIN_NAMESPACE_K_DETAIL_BEGIN

class timer_queue : noncopyable {
  friend class event_loop;

public:
  typedef size_t size_type;
  typedef timer_id::timer_ptr timer_ptr;
  typedef std::vector<timer_ptr> timer_list;

  timer_queue (event_loop *loop);

  ~timer_queue () noexcept;

  timer &
  insert (timestamp initval, timestamp delay, timeout_handler &&handler);

  void
  remove (timer_ptr &ptr);

  size_type
  size () const noexcept {
    return heap_.size();
  }

  bool
  empty () const noexcept {
    return heap_.empty();
  }

  void
  clear ();

  void
  sort ();

  timestamp
  min_delay () const noexcept;

  timer_list &
  get_ready_timers (timer_list &list);

  void
  process_ready_timers (timer_list &list, mutex &m);

private:
#if defined(KNGIN_USE_MONOTONIC_TIMER)
  struct timer_less {
    bool
    operator () (timer_ptr &first, timer_ptr &second) {
      assert(first and second);
      return (first->get_timeout().next() > second->get_timeout().next());
    }
  };
  typedef std::priority_queue<timer_ptr, timer_list, timer_less> min_heap_type;

  min_heap_type heap_;
#elif defined(KNGIN_USE_TIMERFD_TIMER)
  obj_mgr<timer> heap_;
#endif /* defined(KNGIN_USE_MONOTONIC_TIMER) */

  event_loop *loop_;
};

KNGIN_NAMESPACE_K_DETAIL_END

#endif /* KNGIN_TIMER_QUEUE_H */