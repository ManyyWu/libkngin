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
   : inited_(false) {
    reinit(count);
  }

  ~posix_barrier () noexcept {
    if (inited_)
      destroy();
  }

  void
  reinit (int count) {
    assert(!inited_);
    inited_ = true;
  }

  bool
  wait () {
    if (inited_) {
    }
    return false;
  }

  void
  destroy () {
    inited_ = false;
  }

  bool
  destroyed () const noexcept {
    return !inited_;
  }

private:

  std::atomic_bool inited_;
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_POSIX_BARRIER) */

#endif /* KNGIN_POSIX_BARRIER_H */