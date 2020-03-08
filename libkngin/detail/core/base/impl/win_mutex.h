#ifndef KNGIN_WIN_MUTEX_H
#define KNGIN_WIN_MUTEX_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_WIN_MUTEX)

#include "kngin/core/base/timestamp.h"

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class win_mutex {
  friend class cond_impl;

public:
  win_mutex ();

  ~win_mutex () noexcept;

  void
  lock () noexcpet;

  void
  unlock () noexcept;

  bool
  try_lock () noexcept;

private:
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_WIN_MUTEX) */

#endif /* KNGIN_WIN_MUTEX_H */
