#ifndef KNGIN_POSIX_MUTEX_H
#define KNGIN_POSIX_MUTEX_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_POSIX_MUTEX)

#include "kngin/core/base/timestamp.h"
#if defined(KNGIN_SYSTEM_WIN32)
#  include "pthreads_win32/pthread.h"
#else
#  include <pthread.h>
#endif /* defined(KNGIN_SYTEM_WIN32) */

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
  lock () noexcept {
#if !defined(NDEBUG)
    assert(thread::tid() != owner_);
#endif /* !defined(NDEBUG) */
    ::pthread_mutex_lock(&mutex_);
#if !defined(NDEBUG)
    owner_ = thread::tid();
#endif /* !defined(NDEBUG) */
  }

  void
  unlock () noexcept {
    ::pthread_mutex_unlock(&mutex_);
#if !defined(NDEBUG)
    owner_ = 0;
#endif /* !defined(NDEBUG) */
  }

  bool
  try_lock () noexcept {
#if !defined(NDEBUG)
    assert(thread::tid() != owner_);
#endif /* !defined(NDEBUG) */
    auto ec = ::pthread_mutex_trylock(&mutex_);
#if !defined(NDEBUG)
    if (!ec)
      owner_ = thread::tid();
#endif /* !defined(NDEBUG) */
    return (EBUSY != ec);
  }

private:
  pthread_mutex_t mutex_;

#if !defined(NDEBUG)
  std::atomic_uint64_t owner_;
#endif /* !defined(NDEBUG) */
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_POSIX_MUTEX) */

#endif /* KNGIN_POSIX_MUTEX_H */
