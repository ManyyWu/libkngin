#ifndef KNGIN_TIMER_QUEUE_H
#define KNGIN_TIMER_QUEUE_H

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/event/detail.h"
#include "kngin/core/base/mutex.h"
#include "detail/core/event/timer.h"
#include <vector>
#include <queue>

KNGIN_NAMESPACE_K_DETAIL_BEGIN

class timer_queue : noncopyable {
public:
  typedef size_t size_type;
  typedef timer_id::timer_ptr timer_ptr;
  typedef std::vector<timer_ptr> timer_list;

  timer_queue ();

  ~timer_queue ();

  timer &
  insert (timestamp initval, timestamp delay, timeout_handler &&handler);

  void
  remove (timer_ptr &ptr);

  void
  clear ();

  timestamp
  min_delay () const noexcept;

  timer_list &
  get_ready_timers (timer_list &list);

  void
  process_ready_timer (timer_list &list, mutex &m);

private:
  struct timer_less {
    bool
    operator () (timer_ptr &first, timer_ptr &second) {
      return (first->get_timeout().next() < first->get_timeout().next());
    }
  };
  typedef std::priority_queue<timer_ptr, timer_list, timer_less> min_heap_type;

  min_heap_type heap_;
};

KNGIN_NAMESPACE_K_DETAIL_END

#endif /* KNGIN_TIMER_QUEUE_H */