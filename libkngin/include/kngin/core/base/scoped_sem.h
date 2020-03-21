#ifndef SCOPE_SEM_H
#define SCOPE_SEM_H

#include "kngin/core/define.h"
#include "kngin/core/base/noncopyable.h"

KNGIN_NAMESPACE_K_BEGIN

template <typename Tp>
class scope_sem : public noncopyable {
public:
  scope_sem (Tp &sem) noexcept
   : sem_(sem) {
    sem_.wait();
  }

  ~scope_sem () noexcept {
    sem_.post();
  }

private:
  Tp &sem_;
};

KNGIN_NAMESPACE_K_END

#endif /* SCOPE_SEM_H */