#include "kngin/core/base/common.h"
#include "detail/core/event/impl/monotonic_timer.h"

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

void
monotonic_timer::set_time (timestamp now_time, timestamp delay, bool persist) noexcept {
  timeout_.reset(now_time, delay, persist);
  closed_ = false;
}

void
monotonic_timer::close () noexcept {
  timeout_.clear();
  closed_ = true;
}

void
monotonic_timer::on_events () {
  auto s = self();
  if (handler_) {
    TRY()
      handler_(id_);
    CATCH_ERROR("monotonic_timer::handler() error")
  }
}

KNGIN_NAMESPACE_K_DETAIL_IMPL_END
