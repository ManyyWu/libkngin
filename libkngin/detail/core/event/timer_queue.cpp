#include "detail/core/event/timer_queue.h"
#include "detail/core/event/timer.h"

KNGIN_NAMESPACE_K_DETAIL_BEGIN

timer_queue::timer_queue ()
 : timers_(0) {
}

timer_queue::~timer_queue () {
}

KNGIN_NAMESPACE_K_DETAIL_END
