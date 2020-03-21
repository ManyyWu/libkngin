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
#if !defined(NDEBUG)
    assert(thread::tid() != owner_);
#endif /* !defined(NDEBUG) */
    auto ec = ::pthread_mutex_lock(&mutex_);
    if (ec)
      throw_system_error("::pthread_mutex_lock() error", ERRNO(ec));
#if !defined(NDEBUG)
    owner_ = thread::tid();
#endif /* !defined(NDEBUG) */
  }

  void
  unlock () {
    auto ec = ::pthread_mutex_unlock(&mutex_);
    if (ec)
      throw_system_error("::pthread_mutex_unlock() error", ERRNO(ec));
#if !defined(NDEBUG)
    owner_ = 0;
#endif /* !defined(NDEBUG) */
  }

  bool
  try_lock () {
#if !defined(NDEBUG)
    assert(thread::tid() != owner_);
#endif /* !defined(NDEBUG) */
    auto ec = ::pthread_mutex_trylock(&mutex_);
    if (ec and EBUSY != ec)
      throw_system_error("::pthread_mutex_trylock() error", ERRNO(ec));
#if !defined(NDEBUG)
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
