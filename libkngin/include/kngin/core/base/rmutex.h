#ifndef KNGIN_RMUTEX_H
#define KNGIN_RMUTEX_H

#include "kngin/core/base/mutex_base.h"
#include "kngin/core/base/scoped_lock.h"
#include "kngin/core/base/detail.h"

KNGIN_NAMESPACE_K_BEGIN

class rmutex : public mutex_base {
  friend class cond;

public:
  typedef scope_lock<rmutex> scoped_lock;

  rmutex ();

  virtual
  ~rmutex () noexcept;

  virtual
  void
  lock () noexcept;

  virtual
  void
  unlock () noexcept;

  virtual
  bool
  try_lock () noexcept;

private:
  rmutex_impl *impl_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_RMUTEX_H */
