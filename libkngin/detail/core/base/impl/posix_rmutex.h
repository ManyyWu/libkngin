#ifndef KNGIN_POSIX_RMUTEX_H
#define KNGIN_POSIX_RMUTEX_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_POSIX_RMUTEX)

#include "kngin/core/base/timestamp.h"
#include "kngin/core/base/thread.h"
#if defined(KNGIN_SYSTEM_WIN32)
#  include "pthreads_win32/pthread.h"
#else
#  include <pthread.h>
#endif /* defined(KNGIN_SYTEM_WIN32) */

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class posix_rmutex {
  friend class posix_cond;

public:
  posix_rmutex ()
   : rmutex_(),
     attr_() {
    auto ec = ::pthread_mutexattr_init(&attr_);
    if (ec)
      throw_system_error("::pthread_mutexattr_init() error", ERRNO(ec));
    ::pthread_mutexattr_settype(&attr_, PTHREAD_MUTEX_RECURSIVE);
    ec = ::pthread_mutex_init(&rmutex_, &attr_);
    if (ec) {
      ::pthread_mutexattr_destroy(&attr_);
      throw_system_error("::pthread_mutex_init() error", ERRNO(ec));
    }
  }

  ~posix_rmutex () noexcept {
    ::pthread_mutex_destroy(&rmutex_);
    ::pthread_mutexattr_destroy(&attr_);
  }

  void
  lock () noexcept {
    ::pthread_mutex_lock(&rmutex_);
  }

  void
  unlock () noexcept {
    ::pthread_mutex_unlock(&rmutex_);
  }

  bool
  try_lock () noexcept {
    auto ec = ::pthread_mutex_trylock(&rmutex_);
    return (EBUSY != ec);
  }

private:
  pthread_mutex_t rmutex_;

  pthread_mutexattr_t attr_;
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_POSIX_RMUTEX) */

#endif /* KNGIN_POSIX_RMUTEX_H */