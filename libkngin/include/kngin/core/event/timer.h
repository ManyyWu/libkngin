#ifndef KNGIN_TIMER_H
#define KNGIN_TIMER_H

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/event/timer_id.h"

KNGIN_NAMESPACE_K_BEGIN

class timer : noncopyable {

  timer_id *id_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_TIMER_H */
