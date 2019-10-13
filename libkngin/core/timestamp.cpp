#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include <time.h>
#include <algorithm>
#include "timestamp.h"
#include "define.h"

__NAMESPACE_BEGIN

timestamp::timestamp (uint64_t _ms)
    : m_ms(_ms)
{
}

timestamp::timestamp (const timestamp &_t)
    : m_ms(_t.m_ms)
{
}

timestamp::timestamp (const timeval &_tv)
    : m_ms(_tv.tv_sec * 1000 + _tv.tv_usec / 1000)
{
}

timestamp::timestamp (const timespec &_ts)
    : m_ms(_ts.tv_sec * 1000 + _ts.tv_nsec / 1000000)
{
}

timestamp
timestamp::operator = (timestamp _t)
{
    return (m_ms = _t.m_ms);
}

timestamp
timestamp::operator = (uint64_t _t)
{
    return (m_ms = _t);
}

timestamp
timestamp::operator = (timeval _tv)
{
    return (m_ms = _tv.tv_sec * 1000 + _tv.tv_usec / 1000);
}

timestamp
timestamp::operator = (timespec _ts)
{
    return (m_ms = _ts.tv_sec * 1000 + _ts.tv_nsec / 1000000);
}

uint64_t
timestamp::value ()
{
    return m_ms;
}

int
timestamp::value_int ()
{
    return (int)std::min(m_ms, (uint64_t)INT_MAX);
}

void
timestamp::to_timeval (timeval &_tv) const
{
    _tv.tv_sec = m_ms / 1000;
    _tv.tv_usec = 1000 * (m_ms % 1000);
}

void
timestamp::to_timespec (timespec &_ts) const
{
    _ts.tv_sec = m_ms / 1000;
    _ts.tv_nsec = 1000000 * (m_ms % 1000);
}

timestamp
timediff (const timeval &_tvl, const timeval &_tvr)
{
    return (timestamp(_tvl).value() - timestamp(_tvr).value());
}

timestamp
timediff (const timespec &_tsl, const timespec &_tsr)
{
    return (timestamp(_tsl).value() - timestamp(_tsr).value());
}

#ifdef _WIN32
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    FILETIME ft;
    unsigned __int64 tmpres = 0;
    static int tzflag;

    if (NULL != tv)
    {
        GetSystemTimeAsFileTime(&ft);

        tmpres |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;

        /*converting file time to unix epoch*/
        tmpres -= DELTA_EPOCH_IN_MICROSECS;
        tmpres /= 10;  /*convert into microseconds*/
        tv->tv_sec = (long)(tmpres / 1000000UL);
        tv->tv_usec = (long)(tmpres % 1000000UL);
    }

    if (NULL != tz)
    {
        if (!tzflag)
        {
            _tzset();
            tzflag++;
        }
        tz->tz_minuteswest = _timezone / 60;
        tz->tz_dsttime = _daylight;
    }

    return 0;
}

#endif

__NAMESPACE_END
