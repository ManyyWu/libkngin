#ifndef KNGIN_SERVICE_H
#define KNGIN_SERVICE_H

#include "kngin/core/define.h"
#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/thread.h"
#include "kngin/core/event/event_loop.h"
#include <functional>

namespace k {

class service : public noncopyable {
public:
  typedef std::function<void (thread::tid_t)> crash_handler;

  service ();

  ~service () noexcept;

  void
  run (size_t thread_num, crash_handler &&handler);

  void
  stop ();

  event_loop &
  next_loop () noexcept;

private:
  enum {
    flag_stopped  = 0x01,
    flag_stopping = 0x02,
  };

  event_loop loop_;

  std::shared_ptr<thread> thr_;

  // io_threadpool pool_;

  // int flags_;

  // mutex mutex_;
};

} /* namespace  */


#endif /* KNGIN_SERVICE_H */