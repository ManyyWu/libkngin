#ifndef KNGIN_SEMAPHORE_IMPL_H
#define KNGIN_SEMAPHORE_IMPL_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_WIN_SEMAPHORE)
#  include "detail/core/base/impl/win_semaphore.h"
#elif defined(KNGIN_USE_POSIX_SEMAPHORE)
#  include "detail/core/base/impl/posix_semaphore.h"
#endif /* defined(KNGIN_USE_WIN_SEMAPHORE) */

#endif /* KNGIN_SEMAPHORE_IMPL_H */
