#ifndef KNGIN_THREAD_IMPL_H
#define KNGIN_THREAD_IMPL_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_WIN_THREAD)
#  include "detail/core/base/impl/win_thread.h"
#elif defined(KNGIN_USE_POSIX_THREAD)
#  include "detail/core/base/impl/posix_thread.h"
#endif /* defined(KNGIN_USE_WIN_THREAD) */

#endif /* KNGIN_THREAD_IMPL_H */
