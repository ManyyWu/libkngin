#ifndef KNGIN_POSIX_SEMAPHORE_H
#define KNGIN_POSIX_SEMAPHORE_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_POSIX_SEMAPHORE)

#include "kngin/core/base/timestamp.h"
#include "detail/core/base/impl/posix_semaphore.h"
#if defined(KNGIN_SYSTEM_WIN32)
# include "semaphore.h"
#else
# include <sys/sem.h>
#endif /* defined(KNGIN_SYSTEM_WIN32) */

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class posix_semaphore {
public:
  explicit
  posix_semaphore (unsigned initval)
   : sem_(nullptr) {
    if (::sem_init(&sem_, 0, initval) < 0)
      throw_system_error("::sem_init() error", ERRNO(errno));
  }

  ~posix_semaphore () noexcept {
    ::sem_destroy(&sem_);
  }

  bool
  wait () {
    auto ret = ::sem_wait(&sem_);
    if (ret < 0 and ETIMEDOUT != ret)
      throw_system_error("::sem_wait() error", ERRNO(errno));
    return (ETIMEDOUT != ret);
  }

  bool
  timed_wait (timestamp ms) {
    timespec ts;
    ::timespec_get(&ts, TIME_UTC);
    timestamp time = ts;
    (time += ms).to_timespec(ts);
    auto ret = ::sem_timedwait(&sem_, &ts);
    if (ret < 0 and ETIMEDOUT != ret)
      throw_system_error("::sem_timedwait() error", ERRNO(errno));
    return (ETIMEDOUT != ret);
  }

  void
  post () {
    if (::sem_post(&sem_) < 0)
      throw_system_error("::sem_post() error", ERRNO(errno));
  }

private:
  sem_t sem_;
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_POSIX_SEMAPHORE) */

#endif /* KNGIN_POSIX_SEMAPHORE_H */
