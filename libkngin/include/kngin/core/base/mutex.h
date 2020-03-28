#ifndef KNGIN_MUTEX_H
#define KNGIN_MUTEX_H

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/scoped_lock.h"
#include "kngin/core/base/detail.h"

KNGIN_NAMESPACE_K_BEGIN

class mutex : public noncopyable {
  friend class cond;

public:
  typedef scope_lock<mutex> scoped_lock;

  mutex ();

  virtual
  ~mutex () noexcept;

  virtual
  void
  lock ();

  virtual
  void
  unlock ();

  virtual
  bool
  try_lock ();

private:
  mutex_impl *impl_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_MUTEX_H */
