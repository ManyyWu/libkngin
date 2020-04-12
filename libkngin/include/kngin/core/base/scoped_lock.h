#ifndef SCOPE_LOCK_H
#define SCOPE_LOCK_H

#include "kngin/core/define.h"
#include "kngin/core/base/noncopyable.h"

namespace k {

template <typename Tp>
class scope_lock : public noncopyable {
public:
  scope_lock (Tp &lock) noexcept
   : lock_(lock) {
    lock_.lock();
  }

  ~scope_lock () noexcept {
    lock_.unlock();
  }

private:
  Tp &lock_;
};

} /* namespace k */

#endif /* SCOPE_LOCK_H */