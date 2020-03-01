#ifndef KNGIN_TIMEOUT_H
#define KNGIN_TIMEOUT_H

#include "core/base/define.h"
#include "core/base/timestamp.h"

KNGIN_NAMESPACE_K_BEGIN

class timeout {
public:
    timeout      ();

    timeout      (timestamp  interval, bool persist);

    timeout      (timestamp now_time, timestamp  interval, bool persist);

    ~timeout     () = default;

public:
    void
    update       () noexcept
    { m_update_time = timestamp::monotonic(); }

    void
    update       (timestamp timeval) noexcept
    { m_update_time = timeval; }

    void
    clear        () noexcept
    { m_update_time = 0; m_interval = 0; m_persist = false; }

    bool
    timed_out    () const noexcept
    { return (timestamp::monotonic() - m_update_time) >= m_interval; }

    bool
    timed_out    (timestamp now_time) const noexcept
    { return (now_time - m_update_time) >= m_interval; }

    timestamp
    remaining    () const noexcept;

    timestamp
    remaining    (timestamp now_time) const noexcept;

public:
    void
    set_interval (timestamp interval) noexcept
    { m_interval = interval; }

    void
    set_persist  (bool on) noexcept
    { m_persist = on; }

    timestamp
    update_time  () const noexcept
    { return m_update_time; }

    timestamp
    interval     () const noexcept
    { return m_interval; }

    bool
    persist      () const noexcept
    { return m_persist; }

private:
    timestamp m_update_time;

    timestamp m_interval;

    bool      m_persist;
};


KNGIN_NAMESPACE_K_END

#endif /* KNGIN_TIMEOUT_H */