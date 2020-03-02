#ifndef KNGIN_PTHREAD_COND_H
#define KNGIN_PTHREAD_COND_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_PTHREAD_COND)

#include "kngin/core/base/timestamp.h"
#include "kngin/core/base/system_error.h"
#include "detail/core/base/impl/pthread_mutex.h"

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class pthread_cond {
public:
  pthread_cond (mutex_impl &mutex)
      : cond_(PTHREAD_COND_INITIALIZER),
        mutex_(mutex.mutex_){
  }

  ~pthread_cond () noexcept {
    assert(0 == ::pthread_cond_destroy(&cond_));
  }

  void
  wait () {
    assert(0 == ::pthread_cond_wait(&cond_, &mutex_));
  }

  bool
  timed_wait (timestamp ms) {
    timespec ts;
    ::timespec_get(&ts, TIME_UTC);
    (ms += ts).to_timespec(ts);
    auto ec = ::pthread_cond_timedwait(&cond_, &mutex_, &ts);
    if (ETIMEDOUT == ec or (assert(0 == ec), true))
      return false;
    return true;
  }

  void
  signal () {
    assert(0 == ::pthread_cond_signal(&cond_));
  }

  void
  broadcast () {
    assert(0 == ::pthread_cond_broadcast(&cond_));
  }

private:
  pthread_cond_t cond_;

  pthread_mutex_t &mutex_;
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_PTHREAD_COND) */

#endif /* KNGIN_PTHREAD_COND_H */