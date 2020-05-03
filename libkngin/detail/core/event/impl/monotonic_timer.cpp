#include "detail/core/event/impl/monotonic_timer.h"
#if defined(KNGIN_USE_MONOTONIC_TIMER)

#include "kngin/core/base/common.h"

namespace k::detail::impl {

monotonic_timer::monotonic_timer (timeout_handler &&handler,
                                  timestamp initval, timestamp interval) noexcept
 : timeout_(initval, interval),
   handler_(std::move(handler)),
   closed_(false),
   id_() {
}

void
monotonic_timer::set_time (timestamp initval, timestamp interval) noexcept {
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
    IGNORE_EXCP("monotonic_timer::on_events")
  }
}

} /* namespace k::detail::impl */

#endif /* defined(KNGIN_USE_MONOTONIC_TIMER) */
