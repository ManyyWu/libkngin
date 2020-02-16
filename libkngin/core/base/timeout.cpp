#include "core/base/timeout.h"

#define KNGIN_FILENAME "libkngin/core/base/timeout.cpp"

KNGIN_NAMESPACE_K_BEGIN

timeout::timeout ()
    : m_update_time(0),
      m_interval(0),
      m_persist(false)
{
}
timeout::timeout (timestamp  _interval, bool _persist)
    : m_interval(_interval),
      m_persist(_persist)
{
    update();
}

timeout::timeout (timestamp _now_time, timestamp  _interval, bool _persist)
    : m_update_time(_now_time),
      m_interval(_interval),
      m_persist(_persist)
{
}

timestamp
timeout::remaining () const KNGIN_NOEXCP
{
    time_t _diff = timestamp::diff(timestamp::monotonic(), m_update_time);
    return (_diff > m_interval.value() ? 0 : m_interval.value() - _diff);
}

timestamp
timeout::remaining (timestamp _now_time) const KNGIN_NOEXCP
{
    time_t _diff = timestamp::diff(_now_time, m_update_time);
    return (_diff > m_interval.value() ? 0 : m_interval.value() - _diff);
}

KNGIN_NAMESPACE_K_END
