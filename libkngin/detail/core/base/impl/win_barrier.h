#ifndef KNGIN_WIN_BARRIER_H
#define KNGIN_WIN_BARRIER_H

#include "kngin/core/base/noncopyable.h"
#include "detail/core/base/impl/win_thread.h"
#include "detail/core/base/impl/win_cond.h"
#if defined(KNGIN_USE_WIN_BARRIER)

#include <pthread.h>
#include <atomic>

namespace k::detail::impl {

class win_barrier {
public:
  win_barrier (unsigned count)
   : mutex_(nullptr),
     cond_(nullptr),
     count_(count),
     in_(0),
     out_(0) {
    reinit(count);
  }

  ~win_barrier () noexcept {
    if (!destroyed())
      destroy();
  }

  void
  reinit (unsigned count) {
    assert(destroyed());
    try {
      mutex_ = new win_mutex();
      cond_ = new win_cond(*mutex_);
    } catch (...) {
      safe_release(mutex_);
      safe_release(cond_);
      throw;
    }
    count_ = count;
    in_ = 0;
    out_ = 0;
  }

  bool
  wait () {
    if (!destroyed()) {
      mutex_->lock();

      if (++in_ == count_) {
        in_ = 0;
        out_ = count_;
        cond_->signal();
      } else {
        do {
          cond_->wait();
        } while (0 != in_);
      }

      bool last = (0 == --out_);
      if (!last)
        cond_->signal(); // wake next

      mutex_->unlock();
      return last;
    }
    return false;
  }

  void
  destroy () {
    assert(!in_ and !out_);
    safe_release(mutex_);
    safe_release(cond_);
    in_ = 0;
    out_ = 0;
  }

  bool
  destroyed () const noexcept {
    return (!mutex_ and !cond_);
  }

private:
  win_mutex *mutex_;

  win_cond *cond_;

  unsigned count_;

  unsigned in_;

  unsigned out_;
};

} /* namespace k::detail::impl */

#endif /* defined(KNGIN_USE_WIN_BARRIER) */

#endif /* KNGIN_WIN_BARRIER_H */

/**
 * Refer to:
 *   https://github.com/libuv/libuv/blob/v1.x/src/win/thread.c
 *   https://github.com/libuv/libuv/blob/v1.x/src/unix/thread.c
 */
