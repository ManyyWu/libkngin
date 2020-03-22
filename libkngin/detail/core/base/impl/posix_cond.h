#ifndef KNGIN_POSIX_COND_H
#define KNGIN_POSIX_COND_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_POSIX_COND)

#include "kngin/core/base/timestamp.h"
#include "detail/core/base/impl/posix_mutex.h"
#include "detail/core/base/impl/posix_rmutex.h"

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class posix_cond {
public:
  explicit
  posix_cond (posix_mutex &mutex) noexcept
   : cond_(PTHREAD_COND_INITIALIZER),
     mutex_(mutex.mutex_) {
  }

  explicit
  posix_cond (posix_rmutex &mutex) noexcept
   : cond_(PTHREAD_COND_INITIALIZER),
     mutex_(mutex.mutex_) {
  }

  ~posix_cond () noexcept {
    ::pthread_cond_destroy(&cond_);
  }

  void
  wait () {
    auto ec = ::pthread_cond_wait(&cond_, &mutex_);
    if (ec)
      throw_system_error("::pthread_cond_wait() error", ERRNO(ec));
  }

  bool
  timed_wait (timestamp ms) {
    timespec ts;
    ::timespec_get(&ts, TIME_UTC);
    timestamp time = ts;
    (time += ms).to_timespec(ts);
    auto ec = ::pthread_cond_timedwait(&cond_, &mutex_, &ts);
    if (ec and ETIMEDOUT != ec)
      throw_system_error("::pthread_cond_timedwait() error", ERRNO(ec));
    return (ETIMEDOUT != ec);
  }

  void
  signal () {
    auto ec = ::pthread_cond_signal(&cond_);
    if (ec)
      throw_system_error("::pthread_cond_signal() error", ERRNO(ec));
  }

  void
  broadcast () {
    auto ec = ::pthread_cond_broadcast(&cond_);
    if (ec)
      throw_system_error("::pthread_cond_broadcast() error", ERRNO(ec));
  }

private:
  pthread_cond_t cond_;

  pthread_mutex_t &mutex_;
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_POSIX_COND) */

#endif /* KNGIN_POSIX_COND_H */