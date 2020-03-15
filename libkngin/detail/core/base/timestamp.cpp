#include <ctime>
#include <limits>
#include <algorithm>
#include "kngin/core/base/timestamp.h"

KNGIN_NAMESPACE_K_BEGIN

const timestamp timestamp::max = time_t(std::numeric_limits<timer_t>::max());

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
#ifdef KNGIN_SYSTEM_WIN64
  return ::GetTickCount64();
#else
  return ::GetTickCount();
#endif
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

int gettimeofday(struct ::timeval *tv, struct ::timezone *tz) {
  FILETIME ft;
  unsigned __int64 tmpres = 0;
  static int tzflag;

  if (tv) {
    GetSystemTimeAsFileTime(&ft);

    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;

    // converting file time to unix epoch
    tmpres -= DELTA_EPOCH_IN_MICROSECS;
    tmpres /= 10;  // convert into microseconds
    tv->tv_sec = static_cast<long>(tmpres / 1000000UL);
    tv->tv_usec = static_cast<long>(tmpres % 1000000UL);
  }

  if (tz) {
    if (!tzflag) {
      tzset();
      tzflag++;
    }
    tz->tz_minuteswest = _timezone / 60;
    tz->tz_dsttime = _daylight;
  }

  return 0;
}

#endif /* defined(KNGIN_SYSTEM_WIN32) */