#ifndef KNGIN_EVENT_LOOP_H
#define KNGIN_EVENT_LOOP_H

#include "kngin/core/define.h"
#include "kngin/core/base/thread.h"
#include "kngin/core/base/mutex.h"
#include "kngin/core/base/impl.h"
#include "kngin/core/base/callback.h"

typedef_reactor_impl(reactor);

KNGIN_NAMESPACE_K_BEGIN

class event_loop {
public:
  event_loop ();

  ~event_loop ();

  void
  run ();

private:
  typedef std::deque<task> taskq;

  thread::tid_type tid_;

  reactor *reactor_;

  std::atomic_bool looping_;

  std::atomic_bool stop_;

  taskq taskq_;

  mutex taskq_mutex_;

//  timer_queue timerq_;

  mutex timerq_mutex_;

  std::atomic_bool timer_processing_;

  barrier stop_barrier_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_EVENT_LOOP_H */