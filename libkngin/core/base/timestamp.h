#ifndef KNGIN_TIMESTAMP_H
#define KNGIN_TIMESTAMP_H

#ifndef _WIN32
#include <sys/time.h>
#endif
#include <ctime>
#include <cstdint>
#include <cassert>
#include <limits>
#include <algorithm>
#include "core/base/define.h"
#include "core/base/exception.h"

#ifdef _WIN32
#define get_localtime(arg1, arg2) localtime_s((arg1), (arg2))
#else
#define get_localtime(arg1, arg2) localtime_r((arg2), (arg1))
#endif

#ifdef _WIN32
#if defined(_MSC_VER) or defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif
struct timezone
{
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};
int gettimeofday (struct ::timeval *tv, struct ::timezone *tz);
#endif

KNGIN_NAMESPACE_K_BEGIN

class timestamp {
public:
    timestamp   ()
        : m_ms(0) {}
    timestamp   (time_t _ms) KNGIN_NOEXCP
        : m_ms(_ms) {}
    timestamp   (const timestamp &_t) KNGIN_NOEXCP
        : m_ms(_t.m_ms) {}
    timestamp   (const timeval &_tv) KNGIN_NOEXCP
        : m_ms(_tv.tv_sec * 1000 + _tv.tv_usec / 1000) {}
    timestamp   (const timespec &_ts) KNGIN_NOEXCP
        : m_ms(_ts.tv_sec * 1000 + _ts.tv_nsec / 1000000) {}
    ~timestamp  () = default;

public:
    timestamp &
    operator =  (const timestamp &_t) KNGIN_NOEXCP
    { m_ms = _t.m_ms; return *this; }
    timestamp &
    operator =  (time_t _t) KNGIN_NOEXCP
    { m_ms = _t; return *this; }
    timestamp &
    operator =  (const timeval &_tv) KNGIN_NOEXCP
    { m_ms = _tv.tv_sec * 1000 + _tv.tv_usec / 1000; return *this; }
    timestamp &
    operator =  (const timespec &_ts) KNGIN_NOEXCP
    { m_ms = _ts.tv_sec * 1000 + _ts.tv_nsec / 1000000; return *this; }
    timestamp &
    operator += (timestamp _t) KNGIN_NOEXCP
    { m_ms += _t.m_ms; return *this; }
    timestamp &
    operator -= (timestamp _t) KNGIN_NOEXCP
    { m_ms -= _t.m_ms; return *this; }
    timestamp
    operator +  (timestamp _t) KNGIN_NOEXCP
    { return m_ms + _t.m_ms; }
    timestamp
    operator -  (timestamp _t) KNGIN_NOEXCP
    { return m_ms - _t.m_ms; }

public:
    bool
    operator == (timestamp _t) const KNGIN_NOEXCP
    {return _t.m_ms == m_ms; }
    bool
    operator != (timestamp _t) const KNGIN_NOEXCP
    {return _t.m_ms != m_ms; }
    bool
    operator >  (timestamp _t) const KNGIN_NOEXCP
    {return m_ms > _t.m_ms; }
    bool
    operator <  (timestamp _t) const KNGIN_NOEXCP
    {return m_ms < _t.m_ms; }
    bool
    operator >= (timestamp _t) const KNGIN_NOEXCP
    {return m_ms >= _t.m_ms; }
    bool
    operator <= (timestamp _t) const KNGIN_NOEXCP
    {return m_ms <= _t.m_ms; }

public:
    operator
    bool        () const KNGIN_NOEXCP
    { return m_ms; }

    operator
    time_t      () const KNGIN_NOEXCP
    { return m_ms; }

public:
    time_t
    value       () const KNGIN_NOEXCP
    { return m_ms; }
    int32_t
    value_int   () const KNGIN_NOEXCP
    { return static_cast<int>(std::min<time_t>(m_ms, INT32_MAX)); }
    uint32_t
    value_uint  () const KNGIN_NOEXCP
    { return static_cast<uint32_t>(std::min<time_t>(m_ms, UINT32_MAX)); }
    void
    to_timeval  (timeval &_tv) const KNGIN_NOEXCP
    { _tv.tv_sec = long(m_ms / 1000); _tv.tv_usec = long(1000 * (m_ms % 1000)); }
    void
    to_timespec (timespec &_ts) const KNGIN_NOEXCP
    { _ts.tv_sec = long(m_ms / 1000); _ts.tv_nsec = long(1000000 * (m_ms % 1000)); }

public:
    static timestamp
    infinite    () KNGIN_NOEXCP
    { return UINT64_MAX; }

public:
    static timestamp
    time_of_day () KNGIN_NOEXCP
    { timeval _tv; ::gettimeofday(&_tv, nullptr); return _tv; }
#ifndef _WIN32
    static timestamp
    realtime    () KNGIN_NOEXCP
    { timespec _ts; ::clock_gettime(CLOCK_REALTIME, &_ts); return _ts; }
    static timestamp
    monotonic   () KNGIN_NOEXCP
    { timespec _ts; ::clock_gettime(CLOCK_MONOTONIC, &_ts); return _ts; }
    static timestamp
    boot_time   () KNGIN_NOEXCP
    { timespec _ts; ::clock_gettime(CLOCK_BOOTTIME, &_ts); return _ts; }
#else
    static timestamp
    realtime    () KNGIN_NOEXCP
    { timeval _tv; ::gettimeofday(&_tv, nullptr); return _tv; }
    static timestamp
    monotonic   () KNGIN_NOEXCP
#ifdef _WIN64
    { return ::GetTickCount64(); }
#else
    { return ::GetTickCount(); }
#endif
#endif
    static timestamp
    diff        (timestamp _t1, timestamp _t2)
    { if (_t1 < _t2) throw k::exception("t1 < t2"); return (_t1 - _t2); }
    static timestamp
    abs_diff    (timestamp _t1, timestamp _t2) KNGIN_NOEXCP
    { return (abs)(_t1.m_ms - _t2.m_ms); }

private:
    time_t m_ms;
};

inline timestamp
timediff (const timeval &_tvl, const timeval &_tvr) KNGIN_NOEXCP
{
    return (timestamp(_tvl).value() - timestamp(_tvr).value());
}

inline timestamp
timediff (const timespec &_tsl, const timespec &_tsr) KNGIN_NOEXCP
{
    return (timestamp(_tsl).value() - timestamp(_tsr).value());
}

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_TIMESTAMP_H */