#ifndef KNGIN_WIN_RMUTEX_H
#define KNGIN_WIN_RMUTEX_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_WIN_RMUTEX)

#include "detail/core/base/win_utils.h"

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class win_rmutex {
  friend class win_cond;

public:
  win_rmutex () {
    ::InitializeCriticalSection(&mutex_);
#if !defined(NDEBUG)
    stack_ = 0;
#endif /* !defined(NDEBUG) */
  }

  ~win_rmutex () noexcept {
    TRY()
      ::DeleteCriticalSection(&mutex_);
    IGNORE_EXCP();
#if !defined(NDEBUG)
    assert(!stack_);
#endif /* !defined(NDEBUG) */
  }

  void
  lock () {
    ::EnterCriticalSection(&mutex_);
#if !defined(NDEBUG)
    stack_++;
#endif /* !defined(NDEBUG) */

  }

  void
  unlock () {
    ::LeaveCriticalSection(&mutex_);
#if !defined(NDEBUG)
    stack_--;
#endif /* !defined(NDEBUG) */
  }

  bool
  try_lock () {
    auto ret = ::TryEnterCriticalSection(&mutex_);
#if !defined(NDEBUG)
    if (ret)
      stack_++;
#endif /* !defined(NDEBUG) */
    return ret;
  }

private:
  CRITICAL_SECTION mutex_;

#if !defined(NDEBUG)
  std::atomic_size_t stack_;
#endif /* !defined(NDEBUG) */
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_WIN_MUTEX) */

#endif /* KNGIN_WIN_RMUTEX_H */
