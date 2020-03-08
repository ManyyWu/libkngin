#ifndef KNGIN_TIMER_QUEUE_H
#define KNGIN_TIMER_QUEUE_H

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/event/detail.h"
#include "detail/core/base/list.h"
#include <detail/core/event/impl/monotonic_timer.h>

KNGIN_NAMESPACE_K_DETAIL_BEGIN

class timer_queue : noncopyable {
public:
  timer_queue ();

  ~timer_queue ();

private:
  typedef list<timer> timers;

  timers timers_;
};

KNGIN_NAMESPACE_K_DETAIL_END

#endif /* KNGIN_TIMER_QUEUE_H */