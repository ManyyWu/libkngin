#include "kngin/core/base/common.h"
#include "kngin/core/event/event_loop.h"
#include "detail/core/event/impl/monotonic_timer.h"

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

void
monotonic_timer::set_time (timestamp cur_time, timestamp delay, bool persist) {
  timeout_.update(cur_time);
  timeout_.set_interval(delay);
  timeout_.set_persist(persist);
  closed_ = false;
}

void
monotonic_timer::close () {
  timeout_.clear();
  closed_ = true;
}

void
monotonic_timer::on_events (event_loop &loop) {
  auto s = self();
  if (handler_) {
    TRY()
      handler_(*id_);
    CATCH_ERROR("monotonic_timer::handler() error")
  }
}

KNGIN_NAMESPACE_K_DETAIL_IMPL_END
