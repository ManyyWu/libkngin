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
    update       () KNGIN_NOEXCP
    { m_update_time = timestamp::monotonic(); }

    void
    update       (timestamp _timeval) KNGIN_NOEXCP
    { m_update_time = _timeval; }

    void
    clear        () KNGIN_NOEXCP
    { m_update_time = 0; m_interval = 0; m_persist = false; }

    bool
    timed_out    () const KNGIN_NOEXCP
    { return (timestamp::monotonic() - m_update_time) >= m_interval; }

    bool
    timed_out    (timestamp _now_time) const KNGIN_NOEXCP
    { return (_now_time - m_update_time) >= m_interval; }

    timestamp
    remaining    () const KNGIN_NOEXCP;

    timestamp
    remaining    (timestamp _now_time) const KNGIN_NOEXCP;

public:
    void
    set_interval (timestamp _interval) KNGIN_NOEXCP
    { m_interval = _interval; }

    void
    set_persist  (bool _on) KNGIN_NOEXCP
    { m_persist = _on; }

    timestamp
    update_time  () const KNGIN_NOEXCP
    { return m_update_time; }

    timestamp
    interval     () const KNGIN_NOEXCP
    { return m_interval; }

    bool
    persist      () const KNGIN_NOEXCP
    { return m_persist; }

private:
    timestamp m_update_time;

    timestamp m_interval;

    bool      m_persist;
};


KNGIN_NAMESPACE_K_END

#endif /* KNGIN_TIMEOUT_H */