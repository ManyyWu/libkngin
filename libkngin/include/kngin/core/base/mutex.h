#ifndef KNGIN_MUTEX_H
#define KNGIN_MUTEX_H

#include "kngin/core/define.h"
#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/memory.h"
#include "kngin/core/base/scope_lock.h"
#include "kngin/core/base/timestamp.h"

KNGIN_NAMESPACE_K_BEGIN

class mutex_impl;

class mutex : public noncopyable {
  friend class cond;

public:
  typedef scope_lock<mutex> scope_lock;

  mutex ();

  ~mutex () noexcept;

  void
  lock ();

  void
  unlock ();

  bool
  try_lock ();

private:
  typedef mutex_impl impl_type;

  impl_type *impl_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_MUTEX_H */
