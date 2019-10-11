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

#define __time_valid(_t) (TIME_INFINITE == (_t) || (_t) < TIME_MAX)

class timestamp : copyable {
public:
    timestamp    (uint64_t _ms);

    timestamp    (const timestamp &_t);

    timestamp    (const timeval &_tv);

    timestamp    (const timespec &_ts);

public:
    timestamp
    operator =   (timestamp _t);

    timestamp
    operator =   (uint64_t _t);

    timestamp
    operator =   (timeval _tv);

    timestamp
    operator =   (timespec _ts);

public:
    uint64_t
    value        ();

    int
    value_int    ();

    void
    to_timeval   (timeval &_tv);

    void
    to_timespec  (timespec &_ts);

protected:
    uint64_t m_ms;
};

timestamp
timediff (const timeval &_tvl, const timeval &_tvr);

timestamp
timediff (const timespec &_tsl, const timespec &_tsr);

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