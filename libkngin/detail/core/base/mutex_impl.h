#ifndef KNGIN_MUTEX_IMPL_H
#define KNGIN_MUTEX_IMPL_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_WIN_MUTEX)
#  include "detail/core/base/impl/win_mutex.h"
#elif defined(KNGIN_USE_POSIX_MUTEX)
#  include "detail/core/base/impl/posix_mutex.h"
#endif /* defined(KNGIN_USE_WIN_MUTEX) */

#endif /* KNGIN_MUTEX_IMPL_H */
