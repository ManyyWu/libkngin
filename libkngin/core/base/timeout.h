#ifndef KNGIN_TIMEOUT_H
#define KNGIN_TIMEOUT_H

#include "core/base/define.h"
#include "core/base/timestamp.h"

KNGIN_NAMESPACE_K_BEGIN

class timeout {
public:
    timeout      ();

    timeout      (timestamp  _interval, bool _persist);

    timeout      (timestamp _now_time, timestamp  _interval, bool _persist);

    ~timeout     () = default;

public:
    void
    update       () noexcept
    { m_update_time = timestamp::monotonic(); }

    void
    update       (timestamp _timeval) noexcept
    { m_update_time = _timeval; }

    void
    clear        () noexcept
    { m_update_time = 0; m_interval = 0; m_persist = false; }

    bool
    timed_out    () const noexcept
    { return (timestamp::monotonic() - m_update_time) >= m_interval; }

    bool
    timed_out    (timestamp _now_time) const noexcept
    { return (_now_time - m_update_time) >= m_interval; }

    timestamp
    remaining    () const noexcept;

    timestamp
    remaining    (timestamp _now_time) const noexcept;

public:
    void
    set_interval (timestamp _interval) noexcept
    { m_interval = _interval; }

    void
    set_persist  (bool _on) noexcept
    { m_persist = _on; }

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