#ifndef _TIMESTAMP_H_
#define _TIMESTAMP_H_

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include <time.h>
#include <cstdint>
#include <limits>
#include <algorithm>
#include "define.h"
#include "copyable.h"

__NAMESPACE_BEGIN

#ifdef _WIN32
#define __localtime(__arg1, __arg2) localtime_s((__arg1), (__arg2))
#else
#define __localtime(__arg1, __arg2) localtime_r((__arg2), (__arg1))
#endif

#define TIME_INFINITE (time_t)(-1)
#define TIME_MAX      (time_t)(std::numeric_limits<time_t>::max)()

#define __time_valid(__t) (TIME_INFINITE == (__t) || (__t) < TIME_MAX)

class timestamp : copyable {
public:
    timestamp   () = delete;
    timestamp   (uint64_t _ms) : m_ms(_ms) {}
    timestamp   (const timestamp &_t) : m_ms(_t.m_ms) {}
    timestamp   (const timeval &_tv) : m_ms(_tv.tv_sec * 1000 + _tv.tv_usec / 1000) {}
    timestamp   (const timespec &_ts) : m_ms(_ts.tv_sec * 1000 + _ts.tv_nsec / 1000000) {}
    ~timestamp  () = default;

public:
    timestamp &
    operator =  (timestamp _t)        { m_ms = _t.m_ms; return *this; }
    timestamp &
    operator =  (uint64_t _t)         { m_ms = _t; return *this; }
    timestamp &
    operator =  (const timeval &_tv)  { m_ms = _tv.tv_sec * 1000 + _tv.tv_usec / 1000; return *this; }
    timestamp &
    operator =  (const timespec &_ts) { m_ms = _ts.tv_sec * 1000 + _ts.tv_nsec / 1000000; return *this; }
    timestamp &
    operator += (timestamp _t)        { m_ms += _t.m_ms; return *this; }
    timestamp &
    operator -= (timestamp _t)        { m_ms -= _t.m_ms; return *this; }

public:
    uint64_t
    value       () const              { return m_ms; }
    int
    value_int   () const              { return (int)std::min(m_ms, (uint64_t)INT_MAX); }
    void
    to_timeval  (timeval &_tv) const
    { _tv.tv_sec = m_ms / 1000; _tv.tv_usec = 1000 * (m_ms % 1000); }
    void
    to_timespec (timespec &_ts) const
    { _ts.tv_sec = m_ms / 1000; _ts.tv_nsec = 1000000 * (m_ms % 1000); }

protected:
    uint64_t m_ms;
};

inline timestamp
timediff (const timeval &_tvl, const timeval &_tvr)
{
    return (timestamp(_tvl).value() - timestamp(_tvr).value());
}

inline timestamp
timediff (const timespec &_tsl, const timespec &_tsr)
{
    return (timestamp(_tsl).value() - timestamp(_tsr).value());
}

#ifdef _WIN32
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

struct timezone
{
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};

int gettimeofday (struct timeval *tv, struct timezone *tz);
#endif

__NAMESPACE_END

#endif /* _TIMESTAMP_H_ */