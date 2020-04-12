#ifndef KNGIN_MUTEX_H
#define KNGIN_MUTEX_H

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/scoped_lock.h"
#include "kngin/core/base/detail.h"

namespace k {

class mutex : public noncopyable {
  friend class cond;

public:
  typedef scope_lock<mutex> scoped_lock;

  mutex ();

  ~mutex () noexcept;

  void
  lock ();

  void
  unlock ();

  bool
  try_lock ();

private:
  mutex_impl *impl_;
};

} /* namespace k */

#endif /* KNGIN_MUTEX_H */
