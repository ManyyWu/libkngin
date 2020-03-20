#ifndef KNGIN_POSIX_BARRIER_H
#define KNGIN_POSIX_BARRIER_H

#include "kngin/core/base/noncopyable.h"
#if defined(KNGIN_USE_POSIX_BARRIER)

#include <pthread.h>
#include <atomic>

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class posix_barrier {
public:
  posix_barrier (int count)
   : barrier_(),
     inited_(false) {
    reinit(count);
  }

  ~posix_barrier () noexcept {
    if (inited_)
      destroy();
  }

  void
  reinit (int count) {
    assert(!inited_);
    auto ec = ::pthread_barrier_init(&barrier_, nullptr,  count);
    if (ec)
      throw_system_error("::pthread_barrier_init() error", ERRNO(ec));
    inited_ = true;
  }

  bool
  wait () noexcept {
    if (inited_) {
      auto ret = ::pthread_barrier_wait(&barrier_);
      return (PTHREAD_BARRIER_SERIAL_THREAD == ret);
    }
    return false;
  }

  void
  destroy () noexcept {
    ::pthread_barrier_destroy(&barrier_);
    inited_ = false;
  }

  bool
  destroyed () const noexcept {
    return !inited_;
  }

private:
  pthread_barrier_t barrier_;

  std::atomic_bool inited_;
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_POSIX_BARRIER) */

#endif /* KNGIN_POSIX_BARRIER_H */