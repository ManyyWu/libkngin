#include "core/base/timeout.h"

#define KNGIN_FILENAME "libkngin/core/base/timeout.cpp"

KNGIN_NAMESPACE_K_BEGIN

timeout::timeout ()
    : m_update_time(0),
      m_interval(0),
      m_persist(false)
{
}
timeout::timeout (timestamp  interval, bool persist)
    : m_interval(interval),
      m_persist(persist)
{
    update();
}

timeout::timeout (timestamp now_time, timestamp  interval, bool persist)
    : m_update_time(now_time),
      m_interval(interval),
      m_persist(persist)
{
}

timestamp
timeout::remaining () const noexcept
{
    time_t diff = timestamp::diff(timestamp::monotonic(), m_update_time);
    return (diff > m_interval.value() ? 0 : m_interval.value() - diff);
}

timestamp
timeout::remaining (timestamp now_time) const noexcept
{
    time_t diff = timestamp::diff(now_time, m_update_time);
    return (diff > m_interval.value() ? 0 : m_interval.value() - diff);
}

KNGIN_NAMESPACE_K_END
