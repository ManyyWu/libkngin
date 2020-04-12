#ifndef KNGIN_RMUTEX_H
#define KNGIN_RMUTEX_H

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/scoped_lock.h"
#include "kngin/core/base/detail.h"

namespace k {

class rmutex : public noncopyable {
  friend class cond;

public:
  typedef scope_lock<rmutex> scoped_lock;

  rmutex ();

  ~rmutex () noexcept;

  void
  lock ();

  void
  unlock ();

  bool
  try_lock ();

private:
  rmutex_impl *impl_;
};

} /* namespace k */

#endif /* KNGIN_RMUTEX_H */
