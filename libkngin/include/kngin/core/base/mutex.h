#ifndef KNGIN_MUTEX_H
#define KNGIN_MUTEX_H

#include "kngin/core/define.h"
#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/memory.h"
#include "kngin/core/base/scope_lock.h"
#include "kngin/core/base/timestamp.h"
#include "kngin/core/base/impl.h"

TYPEDEF_MUTEX_IMPL(mutex_impl);

KNGIN_NAMESPACE_K_BEGIN

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

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_MUTEX_H */
