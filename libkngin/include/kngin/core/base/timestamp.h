#ifndef KNGIN_TIMESTAMP_H
#define KNGIN_TIMESTAMP_H

#include "kngin/core/base/exception.h"
#include <ctime>
#include <limits>
#include <algorithm>
#ifndef KNGIN_SYSTEM_WIN32
#  include <sys/time.h>
#endif /* defined(KNGIN_SYSTEM_WIN32) */

#if defined(KNGIN_SYSTEM_WIN32)
#  define get_localtime(arg1, arg2) localtime_s((arg1), (arg2))
#else
#  define get_localtime(arg1, arg2) localtime_r((arg2), (arg1))
#endif /* defined(KNGIN_SYSTEM_WIN32) */
#if defined(KNGIN_SYSTEM_WIN32)
#  if defined(_MSC_VER) or defined(_MSC_EXTENSIONS)
#    define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#  define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif /* defined(_MSC_VER) or defined(_MSC_EXTENSIONS) */
  struct timezone
  {
    int tz_minuteswest; // minutes W of Greenwich
    int tz_dsttime;     // type of dst correction
  };

  int gettimeofday (struct ::timeval *tv, struct ::timezone *tz);
#endif /* defined(KNGIN_SYTEM_WIN32) */

KNGIN_NAMESPACE_K_BEGIN

class timestamp {
  friend
  timestamp
  timediff (const timeval &tvl, const timeval &tvr) noexcept;

  friend
  timestamp
  timediff (const timespec &tvl, const timespec &tvr) noexcept;

public:
  timestamp ()
   : ms_(0) {
  }

  timestamp (time_t ms) noexcept
   : ms_(ms) {
  }

  timestamp (const timestamp &t) noexcept
   : ms_(t.ms_) {
  }

  timestamp (const timeval &tv) noexcept
   : ms_(tv.tv_sec * 1000 + tv.tv_usec / 1000) {
  }

  timestamp (const timespec &ts) noexcept
   : ms_(ts.tv_sec * 1000 + ts.tv_nsec / 1000000) {
  }

  ~timestamp () = default;

  timestamp &
  operator = (const timestamp &t) noexcept {
    ms_ = t.ms_;
    return *this;
  }
  timestamp &
  operator = (time_t t) noexcept {
    ms_ = t;
    return *this;
  }
  timestamp &
  operator = (const timeval &tv) noexcept {
    ms_ = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return *this;
  }
  timestamp &
  operator = (const timespec &ts) noexcept {
    ms_ = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    return *this;
  }
  timestamp &
  operator += (timestamp t) noexcept {
    ms_ += t.ms_;
    return *this;
  }
  timestamp &
  operator -= (timestamp t) noexcept {
    ms_ -= t.ms_;
    return *this;
  }
  timestamp
  operator + (timestamp t) noexcept {
    return ms_ + t.ms_;
  }
  timestamp
  operator - (timestamp t) noexcept {
    return ms_ - t.ms_;
  }

  bool
  operator == (timestamp t) const noexcept {
    return t.ms_ == ms_;
  }
  bool
  operator != (timestamp t) const noexcept {
    return t.ms_ != ms_;
  }
  bool
  operator > (timestamp t) const noexcept {
    return ms_ > t.ms_;
  }
  bool
  operator < (timestamp t) const noexcept {
    return ms_ < t.ms_;
  }
  bool
  operator >= (timestamp t) const noexcept {
    return ms_ >= t.ms_;
  }
  bool
  operator <= (timestamp t) const noexcept {
    return ms_ <= t.ms_;
  }

  operator
  bool () const noexcept {
    return ms_;
  }

  operator
  time_t () const noexcept {
    return ms_;
  }

  time_t
  value () const noexcept {
    return ms_;
  }

  int32_t
  value_int () const noexcept {
    return static_cast<int>(std::min<time_t>(ms_, INT32_MAX));
  }

  uint32_t
  value_uint () const noexcept {
    return static_cast<uint32_t>(std::min<time_t>(ms_, UINT32_MAX));
  }

  void
  to_timeval (timeval &tv) const noexcept {
    tv.tv_sec = long(ms_ / 1000);
    tv.tv_usec = long(1000 * (ms_ % 1000));
  }

  void
  to_timespec (timespec &ts) const noexcept {
    ts.tv_sec = long(ms_ / 1000);
    ts.tv_nsec = long(1000000 * (ms_ % 1000));
  }

  static
  timestamp
  infinite () noexcept {
    return UINT64_MAX;
  }

  static
  timestamp
  time_of_day () noexcept {
    timeval tv;
    ::gettimeofday(&tv, nullptr);
    return tv;
  }

#if defined(KNGIN_SYSTEM_WIN32)
  static
  timestamp
  realtime () noexcept;

  static
  timestamp
  monotonic () noexcept;
#else
  static
  timestamp
  realtime () noexcept;

  static
  timestamp
  monotonic () noexcept;
#endif /* defined(KNGIN_SYSTEM_WIN32) */

  static
  timestamp
  diff (timestamp t1, timestamp t2) {
    return (t1 - t2);
  }

  static
  timestamp
  abs_diff (timestamp t1, timestamp t2) noexcept {
    return (abs)(t1.ms_ - t2.ms_);
  }

public:
  static timestamp max;

  static timestamp zero;

private:
  time_t ms_;
};

inline
timestamp
timediff (const timeval &tvl, const timeval &tvr) noexcept {
  return (timestamp(tvl).ms_ - timestamp(tvr).ms_);
}

inline
timestamp
timediff (const timespec &tsl, const timespec &tsr) noexcept {
  return (timestamp(tsl).ms_ - timestamp(tsr).ms_);
}

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_TIMESTAMP_H */