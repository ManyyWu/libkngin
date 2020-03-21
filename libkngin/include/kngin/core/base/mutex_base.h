#ifndef KNGIN_MUTEX_BASE_H
#define KNGIN_MUTEX_BASE_H

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/detail.h"

KNGIN_NAMESPACE_K_BEGIN

class mutex_base : public noncopyable {
  friend class cond;

public:
  virtual
  void
  lock () = 0;

  virtual
  void
  unlock () = 0;

  virtual
  bool
  try_lock () = 0;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_MUTEX_BASE_H */
