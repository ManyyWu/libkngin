#ifndef KNGIN_WIN_SEMAPHORE_H
#define KNGIN_WIN_SEMAPHORE_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_WIN_SEMAPHORE)

#include "kngin/core/base/timestamp.h"
#include "detail/core/base/win_utils.h"
#include "detail/core/base/impl/win_semaphore.h"

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class win_semaphore {
public:
  explicit
  win_semaphore (unsigned initval)
   : sem_(nullptr) {
    sem_ = ::CreateSemaphore(nullptr, initval, LONG_MAX, nullptr);
    if (!sem_)
      throw_system_error("::CreateSemaphore() error", last_error());
  }

  ~win_semaphore () noexcept {
    ::CloseHandle(sem_);
  }

  bool
  wait () {
    auto ret = ::WaitForSingleObject(sem_, INFINITE);
    if (WAIT_OBJECT_0 == ret)
      return true;
    else if (WAIT_TIMEOUT == ret)
      return false;
    else
      throw_system_error("::WaitForSingleObject() error", last_error());
  }

  bool
  timed_wait (timestamp ms) {
    auto ret = ::WaitForSingleObject(sem_, ms.value_uint());
    if (WAIT_OBJECT_0 == ret)
      return true;
    else if (WAIT_TIMEOUT == ret)
      return false;
    else
      throw_system_error("::WaitForSingleObject() error", last_error());
  }

  void
  post () {
    if (FALSE == ::ReleaseSemaphore(sem_, 1, nullptr))
      throw_system_error("::ReleaseSemaphore() error", last_error());
  }

private:
  handle_t sem_;
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_WIN_SEMAPHORE) */

#endif /* KNGIN_WIN_SEMAPHORE_H */
