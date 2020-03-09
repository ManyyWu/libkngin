#ifndef KNGIN_MONOTONIC_TIMER_H
#define KNGIN_MONOTONIC_TIMER_H

#include "kngin/core/base/callback.h"
#include "kngin/core/base/timeout.h"
#include "detail/core/base/list_node.h"
#include "detail/core/base/list.h"
#include <memory>

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class event_loop;
class monotonic_timer
  : public value_base<monotonic_timer>,
    public std::enable_shared_from_this<monotonic_timer> {
  template <typename T,
      typename std::enable_if<std::is_base_of<value_base<T>, T>{}, int>::type>
  friend class list;
  friend class event_loop;

public:
  monotonic_timer (timeout_handler &&handler) noexcept
   : timeout_(),
     handler_(std::move(handler)),
     closed_(false) {
  }

  ~monotonic_timer () noexcept {
    assert(closed_);
  }

  void
  set_time (timestamp now_time, timestamp delay, bool persist) noexcept;

  void
  close () noexcept;

  std::shared_ptr<monotonic_timer>
  self () {
    return shared_from_this();
  }

private:
  void
  on_events (event_loop &loop);

private:
  timeout timeout_;

  timeout_handler handler_;

  std::atomic_bool closed_;

  timer_id *id_;
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* KNGIN_MONOTONIC_TIMER_H */