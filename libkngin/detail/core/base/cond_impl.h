#ifndef KNGIN_COND_IMPL_H
#define KNGIN_COND_IMPL_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_WIN_COND)
#  include "detail/core/base/impl/win_cond.h"
#elif defined(KNGIN_USE_PTHREAD_COND)
#  include "detail/core/base/impl/pthread_cond.h"
#endif /* defined(KNGIN_USE_WIN_COND) */

#endif /* KNGIN_COND_IMPL_H */
