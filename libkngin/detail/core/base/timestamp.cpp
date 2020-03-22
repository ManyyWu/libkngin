#include <ctime>
#include <limits>
#include <algorithm>
#include "kngin/core/base/timestamp.h"

#if defined(KNGIN_SYSTEM_WIN32)
KNGIN_NAMESPACE_GROBLE_BEGIN
extern const LONGLONG g_monotonic_base;
KNGIN_NAMESPACE_GROBLE_END
#endif /* defined(KNGIN_SYSTEM_WIN32) */

KNGIN_NAMESPACE_K_BEGIN

const timestamp timestamp::max = time_t((std::numeric_limits<time_t>::max)());
const timestamp timestamp::zero = time_t(0);

#if defined(KNGIN_SYSTEM_WIN32)
timestamp
timestamp::realtime () noexcept {
  timeval tv;
  ::gettimeofday(&tv, nullptr);
  return tv;
}

timestamp
timestamp::monotonic () noexcept {
#if defined(KNGIN_USE_WINDOWS_GETTICKCOUNT)
#if defined(KNGIN_SYSTEM_WIN64)
  return ::GetTickCount64();
#else
  return ::GetTickCount();
#endif /* defined(KNGIN_SYSTEM_WIN64) */
#else
  LARGE_INTEGER ticks = {0};
  assert(TRUE == ::QueryPerformanceCounter(&ticks));
  return (ticks.QuadPart / g_monotonic_base);
#endif /* defined(KNGIN_USE_WINDOWS_GETTICKCOUNT) */
}
#else
timestamp
timestamp::realtime () noexcept {
  timespec ts;
  ::clock_gettime(CLOCK_REALTIME_COARSE, &ts);
  return ts;
}

timestamp
timestamp::monotonic () noexcept {
  timespec ts;
  ::clock_gettime(CLOCK_MONOTONIC_COARSE, &ts);
  return ts;
}
#endif /* defined(KNGIN_SYSTEM_WIN32) */

KNGIN_NAMESPACE_K_END

#if defined(KNGIN_SYSTEM_WIN32)
int gettimeofday (struct timeval* tp, struct timezone* tzp)
{
    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
    // until 00:00:00 January 1, 1970 
    static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;

    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);
    time = ((uint64_t)file_time.dwLowDateTime);
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec = (long)((time - EPOCH) / 10000000L);
    tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
    return 0;
}
#endif /* defined(KNGIN_SYSTEM_WIN32) */