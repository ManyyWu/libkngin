#ifndef KNGIN_MUTEX_IMPL_H
#define KNGIN_MUTEX_IMPL_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_WIN_MUTEX)
#  include "detail/core/base/impl/win_mutex.h"
#elif defined(KNGIN_USE_PTHREAD_MUTEX)
#  include "detail/core/base/impl/pthread_mutex.h"
#endif /* defined(KNGIN_USE_WIN_MUTEX) */

KNGIN_NAMESPACE_K_BEGIN
KNGIN_NAMESPACE_DETAIL_BEGIN

#if defined(KNGIN_USE_WIN_MUTEX)
  typedef impl::win_mutex mutex_impl;
#elif defined(KNGIN_USE_PTHREAD_MUTEX)
  typedef impl::pthread_mutex mutex_impl;
#endif /* defined(KNGIN_USE_WIN_MUTEX) */

KNGIN_NAMESPACE_DETAIL_END
KNGIN_NAMESPACE_K_END

#endif /* KNGIN_MUTEX_IMPL_H */
