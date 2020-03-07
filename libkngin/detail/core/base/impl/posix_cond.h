#ifndef KNGIN_POSIX_COND_H
#define KNGIN_POSIX_COND_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_POSIX_COND)

#include "kngin/core/base/timestamp.h"
#include "detail/core/base/impl/posix_cond.h"

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class posix_cond {
public:
  explicit
  posix_cond (mutex_impl &mutex) noexcept
   : cond_(PTHREAD_COND_INITIALIZER),
     mutex_(mutex.mutex_){
  }

  ~posix_cond () noexcept {
    ::pthread_cond_destroy(&cond_);
  }

  void
  wait () noexcept {
    ::pthread_cond_wait(&cond_, &mutex_);
  }

  void
  signal () noexcept {
    ::pthread_cond_signal(&cond_);
  }

  void
  broadcast () noexcept {
    ::pthread_cond_broadcast(&cond_);
  }

private:
  pthread_cond_t cond_;

  pthread_mutex_t &mutex_;
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_POSIX_COND) */

#endif /* KNGIN_POSIX_COND_H */