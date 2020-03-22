#ifndef KNGIN_WIN_COND_H
#define KNGIN_WIN_COND_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_WIN_COND)

#include "kngin/core/base/timestamp.h"
#include "detail/core/base/impl/win_mutex.h"
#include "detail/core/base/impl/win_rmutex.h"

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class win_cond {
public:
  explicit
  win_cond (win_mutex &mutex) noexcept
   : cond_(),
     mutex_(mutex.mutex_){
  }

  explicit
  win_cond (win_rmutex &mutex) noexcept
   : cond_(),
     mutex_(mutex.mutex_){
  }

  ~win_cond () noexcept {
    ::InitializeConditionVariable(&cond_);
  }

  void
  wait () {
   ::SleepConditionVariableCS(&cond_, &mutex_, INFINITE);
  }

  bool
  timed_wait (timestamp ms) {
    return ::SleepConditionVariableCS(&cond_, &mutex_, ms.value_uint());
  }

  void
  signal () {
    ::WakeConditionVariable(&cond_);
  }

  void
  broadcast () {
    ::WakeAllConditionVariable(&cond_);
  }

private:
  CONDITION_VARIABLE cond_;

  CRITICAL_SECTION &mutex_;
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_WIN_COND) */

#endif /* KNGIN_WIN_COND_H */