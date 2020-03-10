#include "kngin/core/base/common.h"
#include "detail/core/event/impl/monotonic_timer.h"

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

void
monotonic_timer::set_time (timestamp initval, timestamp interval) noexcept {
  if (id_.cancelled())
    id_ = timer_id(self());
  timeout_.reset(initval, interval);
  closed_ = false;
}

void
monotonic_timer::close () noexcept {
  timeout_.clear();
  closed_ = true;
}

void
monotonic_timer::on_events (const timestamp &now) {
  auto s = self();
  if (handler_) {
    TRY()
      handler_(id_, now);
    CATCH_ERROR("monotonic_timer::handler() error")
  }
}

KNGIN_NAMESPACE_K_DETAIL_IMPL_END
