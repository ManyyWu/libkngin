#ifndef KNGIN_SEMAPHORE_H
#define KNGIN_SEMAPHORE_H

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/timestamp.h"
#include "kngin/core/base/scoped_sem.h"
#include "kngin/core/base/detail.h"

namespace k {

class semaphore : public noncopyable {
public:
  typedef scope_sem<semaphore> scoped_sem;

  explicit
  semaphore (unsigned initval);

  ~semaphore () noexcept;

  bool
  wait ();

  bool
  timed_wait (timestamp ms);

  void
  post ();

private:
  semaphore_impl *impl_;
};

} /* namespace k */

#endif /* KNGIN_SEMAPHORE_H */