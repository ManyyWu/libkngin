#ifndef KNGIN_WIN_MUTEX_H
#define KNGIN_WIN_MUTEX_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_WIN_MUTEX)

#include "detail/core/base/win_utils.h"

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class win_mutex {
  friend class win_cond;

public:
  win_mutex () {
    ::InitializeCriticalSection(&mutex_);
  }

  ~win_mutex () noexcept {
    ::DeleteCriticalSection(&mutex_);
  }

  void
  lock () {
    ::EnterCriticalSection(&mutex_);

  }

  void
  unlock () {
    ::LeaveCriticalSection(&mutex_);
  }

  bool
  try_lock () {
    return ::TryEnterCriticalSection(&mutex_);
  }

private:
  CRITICAL_SECTION mutex_;
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_WIN_MUTEX) */

#endif /* KNGIN_WIN_MUTEX_H */
