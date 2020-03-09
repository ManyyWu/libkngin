#ifndef SCOPE_LOCK_H
#define SCOPE_LOCK_H

#include "kngin/core/define.h"
#include "kngin/core/base/noncopyable.h"

KNGIN_NAMESPACE_K_BEGIN

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

KNGIN_NAMESPACE_K_END

#endif /* SCOPE_LOCK_H */