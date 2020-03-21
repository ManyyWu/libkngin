#ifndef KNGIN_POSIX_RMUTEX_H
#define KNGIN_POSIX_RMUTEX_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_POSIX_RMUTEX)

#include "kngin/core/base/thread.h"
#include <pthread.h>

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class posix_rmutex {
  friend class posix_cond;

public:
  posix_rmutex ()
   : mutex_(),
     attr_() {
    auto ec = ::pthread_mutexattr_init(&attr_);
    if (ec)
      throw_system_error("::pthread_mutexattr_init() error", ERRNO(ec));
    ::pthread_mutexattr_settype(&attr_, PTHREAD_MUTEX_RECURSIVE);
    ec = ::pthread_mutex_init(&mutex_, &attr_);
    if (ec) {
      ::pthread_mutexattr_destroy(&attr_);
      throw_system_error("::pthread_mutex_init() error", ERRNO(ec));
    }
#if !defined(NDEBUG)
    stack_ = 0;
#endif /* !defined(NDEBUG) */
  }

  ~posix_rmutex () noexcept {
    ::pthread_mutex_destroy(&mutex_);
    ::pthread_mutexattr_destroy(&attr_);
#if !defined(NDEBUG)
    assert(!stack_);
#endif /* !defined(NDEBUG) */
  }

  void
  lock () {
    auto ec = ::pthread_mutex_lock(&mutex_);
    if (ec)
      throw_system_error("::pthread_mutex_lock() error", ERRNO(ec));
#if !defined(NDEBUG)
    stack_++;
#endif /* !defined(NDEBUG) */
  }

  void
  unlock () {
    auto ec = ::pthread_mutex_unlock(&mutex_);
    if (ec)
      throw_system_error("::pthread_mutex_unlock() error", ERRNO(ec));
#if !defined(NDEBUG)
    stack_--;
#endif /* !defined(NDEBUG) */
  }

  bool
  try_lock () {
    auto ec = ::pthread_mutex_trylock(&mutex_);
    if (ec and EBUSY != ec)
      throw_system_error("::pthread_mutex_trylock() error", ERRNO(ec));
#if !defined(NDEBUG)
    stack_++;
#endif /* !defined(NDEBUG) */
    return (EBUSY != ec);
  }

private:
  pthread_mutex_t mutex_;

  pthread_mutexattr_t attr_;

#if !defined(NDEBUG)
  std::atomic_size_t stack_;
#endif /* !defined(NDEBUG) */
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_POSIX_RMUTEX) */

#endif /* KNGIN_POSIX_RMUTEX_H */