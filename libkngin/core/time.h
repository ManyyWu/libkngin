#ifndef _TIME_H_
#define _TIME_H_

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include <ctime>
#include <cstdint>
#include <limits>
#include "define.h"

__NAMESPACE_BEGIN

#ifdef _WIN32
#define __localtime(__arg1, __arg2) localtime_s((__arg1), (__arg2))
#else
#define __localtime(__arg1, __arg2) localtime_r((__arg2), (__arg1))
#endif

#define TIME_INFINITE (time_t)(-1)
#define TIME_MAX      (time_t)(std::numeric_limits<time_t>::max)()

#define __time_valid(_t) (TIME_INFINITE == (_t) || (_t) < TIME_MAX)

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

//enum TIME_FMT {
//    TIEM_FMT_YYMMDD = 0,
//    TIME_FMT_YYYYMMDD,
//    TIME_FMT_YYYYMMDDHHmm,
//    TIME_FMT_YYYYMMDDHHmmSS,
//    TIME_FMT_YYYYMMDDHHmmSSms,
//    TIME_FMT_HHmm,
//    TIME_FMT_HHmmSS,
//    TIME_FMT_HHmmSSms,
//    TIME_FMT_SECONDS,
//    TIME_FMT_WEEKS,
//};
//
//enum US_TIME_FMT {
//    US_TIME_FMT_SSmsus = 0,
//    US_TIME_FMT_SSms,
//    US_TIME_FMT_ms,
//};
//
//class time {
//public:
//    static tm
//    localtime    ();
//
//    static timeval
//    localustime  ();
//
//public:
//    static uint16_t
//    year         (TIME_FMT _fmt, int64_t _t);
//
//    static uint8_t
//    month        (TIME_FMT _fmt, int64_t _t);
//
//    static uint8_t
//    day          (TIME_FMT _fmt, int64_t _t);
//
//    static uint8_t
//    hour         (TIME_FMT _fmt, int64_t _t);
//
//    static uint8_t
//    minute       (TIME_FMT _fmt, int64_t _t);
//
//    static uint8_t
//    second       (TIME_FMT _fmt, int64_t _t);
//
//    static uint16_t
//    ms           (TIME_FMT _fmt, int64_t _t);
//
//    static uint8_t
//    second       (US_TIME_FMT _fmt, int64_t _t);
//
//    static uint16_t
//    ms           (US_TIME_FMT _fmt, int64_t _t);
//
//    static uint16_t 
//    us           (US_TIME_FMT _fmt, int64_t _t);
//};

__NAMESPACE_END

#endif /* _TIME_H_ */