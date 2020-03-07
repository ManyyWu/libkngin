#ifndef KNGIN_TIMEOUT_H
#define KNGIN_TIMEOUT_H

#include "kngin/core/base/timestamp.h"

KNGIN_NAMESPACE_K_BEGIN

class timeout {
public:
  timeout ();

  timeout (timestamp  interval, bool persist);

  timeout (timestamp now_time, timestamp  interval, bool persist);

  ~timeout () = default;

public:
  void
  update () noexcept {
    update_time_ = timestamp::monotonic();
  }

  void
  update (timestamp timeval) noexcept {
    update_time_ = timeval;
  }

  void
  clear () noexcept {
    update_time_ = 0;
    interval_ = 0;
    persist_ = false;
  }

  bool
  timed_out () const noexcept {
    return (timestamp::monotonic() - update_time_) >= interval_;
  }

  bool
  timed_out (timestamp now_time) const noexcept {
    return (now_time - update_time_) >= interval_;
  }

  timestamp
  remaining () const noexcept;

  timestamp
  remaining (timestamp now_time) const noexcept;

public:
  void
  set_interval (timestamp interval) noexcept {
    interval_ = interval;
  }

  void
  set_persist (bool on) noexcept {
    persist_ = on;
  }

  timestamp
  update_time () const noexcept {
    return update_time_;
  }

  timestamp
  interval () const noexcept {
    return interval_;
  }

  bool
  persist () const noexcept {
    return persist_;
  }

private:
  timestamp update_time_;

  timestamp interval_;

  bool persist_;
};


KNGIN_NAMESPACE_K_END

#endif /* KNGIN_TIMEOUT_H */