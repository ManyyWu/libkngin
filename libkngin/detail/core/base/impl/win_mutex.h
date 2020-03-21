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
#if !defined(NDEBUG)
    assert(thread::tid() != owner_);
#endif /* !defined(NDEBUG) */
    ::InitializeCriticalSection(&mutex_);
#if !defined(NDEBUG)
    owner_ = thread::tid();
#endif /* !defined(NDEBUG) */
  }

  ~win_mutex () noexcept {
    TRY()
      ::DeleteCriticalSection(&mutex_);
    IGNORE_EXCP();
  }

  void
  lock () {
#if !defined(NDEBUG)
    assert(thread::tid() != owner_);
#endif /* !defined(NDEBUG) */
    ::EnterCriticalSection(&mutex_);
#if !defined(NDEBUG)
    owner_ = thread::tid();
#endif /* !defined(NDEBUG) */

  }

  void
  unlock () {
    ::LeaveCriticalSection(&mutex_);
#if !defined(NDEBUG)
    owner_ = 0;
#endif /* !defined(NDEBUG) */
  }

  bool
  try_lock () {
#if !defined(NDEBUG)
    assert(thread::tid() != owner_);
#endif /* !defined(NDEBUG) */
    auto ret = ::TryEnterCriticalSection(&mutex_);
#if !defined(NDEBUG)
    owner_ = thread::tid();
#endif /* !defined(NDEBUG) */
    return ret;
  }

private:
  CRITICAL_SECTION mutex_;

#if !defined(NDEBUG)
  std::atomic_uint64_t owner_;
#endif /* !defined(NDEBUG) */
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_WIN_MUTEX) */

#endif /* KNGIN_WIN_MUTEX_H */
