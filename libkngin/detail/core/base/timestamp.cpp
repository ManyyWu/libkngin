#include <ctime>
#include <algorithm>
#include "kngin/core/base/timestamp.h"

#define KNGIN_FILENAME "libkngin/core/base/timestamp.cpp"

KNGIN_NAMESPACE_K_BEGIN

KNGIN_NAMESPACE_K_END

#ifdef _WIN32

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

#endif