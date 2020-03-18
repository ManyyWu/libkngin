#ifndef KNGIN_RMUTEX_IMPL_H
#define KNGIN_RMUTEX_IMPL_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_WIN_RMUTEX)
#  include "detail/core/base/impl/win_rmutex.h"
#elif defined(KNGIN_USE_POSIX_RMUTEX)
#  include "detail/core/base/impl/posix_rmutex.h"
#endif /* defined(KNGIN_USE_WIN_RMUTEX) */

#endif /* KNGIN_RMUTEX_IMPL_H */
