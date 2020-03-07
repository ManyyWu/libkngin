#ifndef KNGIN_BARRIER_IMPL_H
#define KNGIN_BARRIER_IMPL_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_WIN_BARRIER)
#  include "detail/core/base/impl/win_barrier.h"
#elif defined(KNGIN_USE_POSIX_BARRIER)
#  include "detail/core/base/impl/posix_barrier.h"
#endif /* defined(KNGIN_USE_WIN_BARRIER) */

#endif /* KNGIN_BARRIER_IMPL_H */
