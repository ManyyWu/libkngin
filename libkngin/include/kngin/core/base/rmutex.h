#ifndef KNGIN_RMUTEX_H
#define KNGIN_RMUTEX_H

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/scoped_lock.h"
#include "kngin/core/base/detail.h"

KNGIN_NAMESPACE_K_BEGIN

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

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_RMUTEX_H */
