#ifndef KNGIN_TIMEOUT_H
#define KNGIN_TIMEOUT_H

#include "kngin/core/base/timestamp.h"

namespace k {

class timeout {
public:
  timeout () noexcept
   : next_(0),
     interval_(0) {
  }

  timeout (timestamp initval, timestamp interval) noexcept
   : next_(initval),
     interval_(interval) {
  }

  ~timeout () = default;

public:
  void
  update () noexcept {
    next_ += interval_;
  }

  void
  clear () noexcept {
    next_ = 0;
    interval_ = 0;
  }

  bool
  timed_out () const noexcept {
    return (timestamp::monotonic() >= next_);
  }

  bool
  timed_out (timestamp now) const noexcept {
    return (now >= next_);
  }

  timestamp
  remaining () const noexcept {
    auto diff = timestamp::diff(next_, timestamp::monotonic());
    return (diff > timestamp::zero ? diff : timestamp::zero);
  }

  timestamp
  remaining (timestamp now) const noexcept {
    auto diff = timestamp::diff(next_, now);
    return (diff > timestamp::zero ? diff : timestamp::zero);
  }

  void
  reset (timestamp initval, timestamp  interval) noexcept {
    next_ = initval;
    interval_ = interval;
  }

  timestamp
  next () const noexcept {
    return next_;
  }

  timestamp
  interval () const noexcept {
    return interval_;
  }

  bool
  persist () const noexcept {
    return interval_;
  }

private:
  timestamp next_;

  timestamp interval_;
};


} /* namespace k */

#endif /* KNGIN_TIMEOUT_H */