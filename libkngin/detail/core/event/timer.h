#ifndef KNGIN_TIMER_H
#define KNGIN_TIMER_H

#include "kngin/core/define.h"

#if defined(KNGIN_USE_TIMERFD_TIMER)
#  include "detail/core/event/impl/timerfd_timer.h"
#elif defined(KNGIN_USE_MONOTONIC_TIMER)
#  include "detail/core/event/impl/monotonic_timer.h"
#elif defined(KNGIN_USE_QUEUED_TIMER)
#  include "detail/core/event/impl/queued_timer.h"
#endif /* defined(KNGIN_USE_TIMERFD_TIMER) */

#endif /* KNGIN_TIMER_H */