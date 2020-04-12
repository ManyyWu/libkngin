#ifndef SCOPE_SEM_H
#define SCOPE_SEM_H

#include "kngin/core/define.h"
#include "kngin/core/base/noncopyable.h"

namespace k {

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

} /* namespace k */

#endif /* SCOPE_SEM_H */