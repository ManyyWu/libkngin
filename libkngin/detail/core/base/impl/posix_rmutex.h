#ifndef KNGIN_POSIX_RMUTEX_H
#define KNGIN_POSIX_RMUTEX_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_POSIX_RMUTEX)

#include "kngin/core/base/thread.h"
#include <pthread.h>

namespace k::detail::impl {

class posix_rmutex {
  friend class posix_cond;

public:
  posix_rmutex ()
   : mutex_() {
    pthread_mutexattr_t attr;
    auto ec = ::pthread_mutexattr_init(&attr);
    if (ec)
      throw_system_error("::pthread_mutexattr_init() error", KNGIN_ERRNO(ec));
    ::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    ec = ::pthread_mutex_init(&mutex_, &attr);
    ::pthread_mutexattr_destroy(&attr);
    if (ec) {
      throw_system_error("::pthread_mutex_init() error", KNGIN_ERRNO(ec));
    }
#if !defined(NDEBUG)
    stack_ = 0;
#endif /* !defined(NDEBUG) */
  }

  ~posix_rmutex () noexcept {
    ::pthread_mutex_destroy(&mutex_);
#if !defined(NDEBUG)
    assert(!stack_);
#endif /* !defined(NDEBUG) */
  }

  void
  lock () {
    auto ec = ::pthread_mutex_lock(&mutex_);
    if (ec)
      throw_system_error("::pthread_mutex_lock() error", KNGIN_ERRNO(ec));
#if !defined(NDEBUG)
    stack_++;
#endif /* !defined(NDEBUG) */
  }

  void
  unlock () {
    auto ec = ::pthread_mutex_unlock(&mutex_);
    if (ec)
      throw_system_error("::pthread_mutex_unlock() error", KNGIN_ERRNO(ec));
#if !defined(NDEBUG)
    stack_--;
#endif /* !defined(NDEBUG) */
  }

  bool
  try_lock () {
    auto ec = ::pthread_mutex_trylock(&mutex_);
    if (ec and EBUSY != ec)
      throw_system_error("::pthread_mutex_trylock() error", KNGIN_ERRNO(ec));
#if !defined(NDEBUG)
    stack_++;
#endif /* !defined(NDEBUG) */
    return (EBUSY != ec);
  }

private:
  pthread_mutex_t mutex_;

#if !defined(NDEBUG)
  std::atomic_size_t stack_;
#endif /* !defined(NDEBUG) */
};

} /* namespace k::detail::impl */

#endif /* defined(KNGIN_USE_POSIX_RMUTEX) */

#endif /* KNGIN_POSIX_RMUTEX_H */