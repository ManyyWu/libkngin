#ifndef KNGIN_POSIX_SEMAPHORE_H
#define KNGIN_POSIX_SEMAPHORE_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_POSIX_SEMAPHORE)

#include "kngin/core/base/timestamp.h"
#include "detail/core/base/impl/posix_semaphore.h"
#if defined(KNGIN_SYSTEM_WIN32)
# include "semaphore.h"
#else
# include <semaphore.h>
#endif /* defined(KNGIN_SYSTEM_WIN32) */

namespace k::detail::impl {

class posix_semaphore {
public:
  explicit
  posix_semaphore (unsigned initval)
   : sem_() {
    if (::sem_init(&sem_, 0, initval) < 0)
      throw_system_error("::sem_init() error", KNGIN_ERRNO(errno));
  }

  ~posix_semaphore () noexcept {
    ::sem_destroy(&sem_);
  }

  bool
  wait () {
    auto ret = ::sem_wait(&sem_);
    if (ret < 0 and ETIMEDOUT != ret)
      throw_system_error("::sem_wait() error", KNGIN_ERRNO(errno));
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
      throw_system_error("::sem_timedwait() error", KNGIN_ERRNO(errno));
    return (ETIMEDOUT != ret);
  }

  void
  post () {
    if (::sem_post(&sem_) < 0)
      throw_system_error("::sem_post() error", KNGIN_ERRNO(errno));
  }

private:
  sem_t sem_;
};

} /* namespace k::detail::impl */

#endif /* defined(KNGIN_USE_POSIX_SEMAPHORE) */

#endif /* KNGIN_POSIX_SEMAPHORE_H */
