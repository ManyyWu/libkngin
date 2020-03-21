#ifndef KNGIN_POSIX_MUTEX_H
#define KNGIN_POSIX_MUTEX_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_POSIX_MUTEX)

#include "kngin/core/base/thread.h"
#include <pthread.h>

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class posix_mutex {
  friend class posix_cond;

public:
  posix_mutex () noexcept
   : mutex_(PTHREAD_MUTEX_INITIALIZER) {
  }

  ~posix_mutex () noexcept {
    ::pthread_mutex_destroy(&mutex_);
  }

  void
  lock () {
    auto ec = ::pthread_mutex_lock(&mutex_);
    if (ec)
      throw_system_error("::pthread_mutex_lock() error", ERRNO(ec));
  }

  void
  unlock () {
    auto ec = ::pthread_mutex_unlock(&mutex_);
    if (ec)
      throw_system_error("::pthread_mutex_unlock() error", ERRNO(ec));
  }

  bool
  try_lock () {
    auto ec = ::pthread_mutex_trylock(&mutex_);
    if (ec and EBUSY != ec)
      throw_system_error("::pthread_mutex_trylock() error", ERRNO(ec));
    return (EBUSY != ec);
  }

private:
  pthread_mutex_t mutex_;
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_POSIX_MUTEX) */

#endif /* KNGIN_POSIX_MUTEX_H */
