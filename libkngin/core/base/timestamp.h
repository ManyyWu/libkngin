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
    timestamp   (time_t ms) noexcept
        : m_ms(ms) {}
    timestamp   (const timestamp &t) noexcept
        : m_ms(t.m_ms) {}
    timestamp   (const timeval &tv) noexcept
        : m_ms(tv.tv_sec * 1000 + tv.tv_usec / 1000) {}
    timestamp   (const timespec &ts) noexcept
        : m_ms(ts.tv_sec * 1000 + ts.tv_nsec / 1000000) {}
    ~timestamp  () = default;

public:
    timestamp &
    operator =  (const timestamp &t) noexcept
    { m_ms = t.m_ms; return *this; }
    timestamp &
    operator =  (time_t t) noexcept
    { m_ms = t; return *this; }
    timestamp &
    operator =  (const timeval &tv) noexcept
    { m_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000; return *this; }
    timestamp &
    operator =  (const timespec &ts) noexcept
    { m_ms = ts.tv_sec * 1000 + ts.tv_nsec / 1000000; return *this; }
    timestamp &
    operator += (timestamp t) noexcept
    { m_ms += t.m_ms; return *this; }
    timestamp &
    operator -= (timestamp t) noexcept
    { m_ms -= t.m_ms; return *this; }
    timestamp
    operator +  (timestamp t) noexcept
    { return m_ms + t.m_ms; }
    timestamp
    operator -  (timestamp t) noexcept
    { return m_ms - t.m_ms; }

public:
    bool
    operator == (timestamp t) const noexcept
    {return t.m_ms == m_ms; }
    bool
    operator != (timestamp t) const noexcept
    {return t.m_ms != m_ms; }
    bool
    operator >  (timestamp t) const noexcept
    {return m_ms > t.m_ms; }
    bool
    operator <  (timestamp t) const noexcept
    {return m_ms < t.m_ms; }
    bool
    operator >= (timestamp t) const noexcept
    {return m_ms >= t.m_ms; }
    bool
    operator <= (timestamp t) const noexcept
    {return m_ms <= t.m_ms; }

public:
    operator
    bool        () const noexcept
    { return m_ms; }

    operator
    time_t      () const noexcept
    { return m_ms; }

public:
    time_t
    value       () const noexcept
    { return m_ms; }
    int32_t
    value_int   () const noexcept
    { return static_cast<int>(std::min<time_t>(m_ms, INT32_MAX)); }
    uint32_t
    value_uint  () const noexcept
    { return static_cast<uint32_t>(std::min<time_t>(m_ms, UINT32_MAX)); }
    void
    to_timeval  (timeval &tv) const noexcept
    { tv.tv_sec = long(m_ms / 1000); tv.tv_usec = long(1000 * (m_ms % 1000)); }
    void
    to_timespec (timespec &ts) const noexcept
    { ts.tv_sec = long(m_ms / 1000); ts.tv_nsec = long(1000000 * (m_ms % 1000)); }

public:
    static timestamp
    infinite    () noexcept
    { return UINT64_MAX; }

public:
    static timestamp
    time_of_day () noexcept
    { timeval tv; ::gettimeofday(&tv, nullptr); return tv; }
#ifndef _WIN32
    static timestamp
    realtime    () noexcept
    { timespec ts; ::clock_gettime(CLOCK_REALTIME, &ts); return ts; }
    static timestamp
    monotonic   () noexcept
    { timespec ts; ::clock_gettime(CLOCK_MONOTONIC, &ts); return ts; }
    static timestamp
    boot_time   () noexcept
    { timespec ts; ::clock_gettime(CLOCK_BOOTTIME, &ts); return ts; }
#else
    static timestamp
    realtime    () noexcept
    { timeval tv; ::gettimeofday(&tv, nullptr); return tv; }
    static timestamp
    monotonic   () noexcept
#ifdef _WIN64
    { return ::GetTickCount64(); }
#else
    { return ::GetTickCount(); }
#endif
#endif
    static timestamp
    diff        (timestamp t1, timestamp t2)
    { if (t1 < t2) throw k::exception("t1 < t2"); return (t1 - t2); }
    static timestamp
    abs_diff    (timestamp t1, timestamp t2) noexcept
    { return (abs)(t1.m_ms - t2.m_ms); }

private:
    time_t m_ms;
};

inline
timestamp
timediff (const timeval &tvl, const timeval &tvr) noexcept
{
    return (timestamp(tvl).value() - timestamp(tvr).value());
}

inline
timestamp
timediff (const timespec &tsl, const timespec &tsr) noexcept
{
    return (timestamp(tsl).value() - timestamp(tsr).value());
}

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_TIMESTAMP_H */