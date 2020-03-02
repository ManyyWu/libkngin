#ifndef KNGIN_PTHREAD_MUTEX_H
#define KNGIN_PTHREAD_MUTEX_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_PTHREAD_MUTEX)

#include "kngin/core/base/timestamp.h"
#include "kngin/core/base/system_error.h"
#if defined(KNGIN_SYSTEM_WIN32)
#  include "kngin/core/base/pthreads_win32/pthread.h"
#else
#  include <pthread.h>
#endif /* defined(KNGIN_SYTEM_WIN32) */

KNGIN_NAMESPACE_K_BEGIN
KNGIN_NAMESPACE_DETAIL_BEGIN
KNGIN_NAMESPACE_IMPL_BEGIN

class pthread_mutex {
  friend class cond_impl;

public:
  pthread_mutex ()
   : mutex_(PTHREAD_MUTEX_INITIALIZER) {
  }

  ~pthread_mutex () noexcept {
    assert(0 == ::pthread_mutex_destroy(&mutex_));
  }

  void
  lock () {
    assert(0 == ::pthread_mutex_lock(&mutex_));
  }

  void
  unlock () {
    assert(0 == ::pthread_mutex_unlock(&mutex_));
  }

  bool
  try_lock () {
    auto ec = ::pthread_mutex_trylock(&mutex_);
    if (EBUSY == ec or (assert(0 == ec), true))
      return false;
    return true;
  }

  bool
  timeed_lock (timestamp ms) {
    timespec ts;
    ::timespec_get(&ts, TIME_UTC);
    (ms += ts).to_timespec(ts);
    auto ec = ::pthread_mutex_timedlock(&mutex_, &ts);
    if (ETIMEDOUT == ec or (assert(0 == ec), true))
      return false;
    return true;
  }

private:
  pthread_mutex_t mutex_;
};

KNGIN_NAMESPACE_IMPL_END
KNGIN_NAMESPACE_DETAIL_END
KNGIN_NAMESPACE_K_END

#endif /* defined(KNGIN_USE_PTHREAD_MUTEX) */

#endif /* KNGIN_PTHREAD_MUTEX_H */
