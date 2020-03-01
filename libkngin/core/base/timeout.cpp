#include "core/base/timeout.h"

#define KNGIN_FILENAME "libkngin/core/base/timeout.cpp"

KNGIN_NAMESPACE_K_BEGIN

timeout::timeout ()
    : update_time_(0),
      interval_(0),
      persist_(false)
{
}
timeout::timeout (timestamp  interval, bool persist)
    : interval_(interval),
      persist_(persist)
{
    update();
}

timeout::timeout (timestamp now_time, timestamp  interval, bool persist)
    : update_time_(now_time),
      interval_(interval),
      persist_(persist)
{
}

timestamp
timeout::remaining () const noexcept
{
    time_t diff = timestamp::diff(timestamp::monotonic(), update_time_);
    return (diff > interval_.value() ? 0 : interval_.value() - diff);
}

timestamp
timeout::remaining (timestamp now_time) const noexcept
{
    time_t diff = timestamp::diff(now_time, update_time_);
    return (diff > interval_.value() ? 0 : interval_.value() - diff);
}

KNGIN_NAMESPACE_K_END
