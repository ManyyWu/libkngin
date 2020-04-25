#ifndef KNGIN_POSIX_BARRIER_H
#define KNGIN_POSIX_BARRIER_H

#include "kngin/core/base/noncopyable.h"
#if defined(KNGIN_USE_POSIX_BARRIER)

#include <pthread.h>
#include <atomic>

namespace k::detail::impl {

class posix_barrier {
public:
  posix_barrier (unsigned count)
   : inited_(false) {
    reinit(count);
  }

  ~posix_barrier () noexcept {
    if (inited_)
      destroy();
  }

  void
  reinit (unsigned count) {
    assert(!inited_);
    auto ec = ::pthread_barrier_init(&barrier_, nullptr,  count);
    if (ec)
      throw_system_error("::pthread_barrier_init() error", KNGIN_ERRNO(ec));
    inited_ = true;
  }

  bool
  wait () {
    if (inited_) {
      auto ec = ::pthread_barrier_wait(&barrier_);
      if (ec and PTHREAD_BARRIER_SERIAL_THREAD != ec)
        throw_system_error("::pthread_barrier_wait() error", KNGIN_ERRNO(ec));
      return (PTHREAD_BARRIER_SERIAL_THREAD == ec);
    }
    return false;
  }

  void
  destroy () {
    auto ec = ::pthread_barrier_destroy(&barrier_);
    if (ec)
      throw_system_error("::pthread_barrier_destroy() error", KNGIN_ERRNO(ec));
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

} /* namespace k::detail::impl */

#endif /* defined(KNGIN_USE_POSIX_BARRIER) */

#endif /* KNGIN_POSIX_BARRIER_H */