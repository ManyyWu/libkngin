#ifndef KNGIN_TIMER_QUEUE_H
#define KNGIN_TIMER_QUEUE_H

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/event/detail.h"
#include "kngin/core/base/mutex.h"
#include "detail/core/event/timer.h"
#include <deque>

KNGIN_NAMESPACE_K_DETAIL_BEGIN

class timer_queue : noncopyable {
public:
  typedef size_t size_type;
  typedef timer_id::timer_ptr timer_ptr;

  timer_queue ();

  ~timer_queue ();

  timer &
  insert (timestamp now_time, timestamp delay, bool persist,
          timeout_handler &&handler);

  void
  remove (timer_ptr &timer);

  timestamp
  process_ready_timer ();

private:
  typedef list<timer> timer_list;

  timer_list timers_;

  mutex mutex_;
};

KNGIN_NAMESPACE_K_DETAIL_END

#endif /* KNGIN_TIMER_QUEUE_H */