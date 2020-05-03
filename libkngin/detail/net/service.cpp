#include "kngin/net/service.h"

namespace k {

service::service ()
 : loop_() {
}

service::~service () noexcept {
  thr_->join();
}

void
service::run (size_t thread_num, crash_handler &&handler) {
  thr_ = std::make_shared<thread>([this] () -> int { loop_.run(); return 0; });
}

void
service::stop () {
  loop_.stop();
}

event_loop &
service::next_loop () noexcept {
  // throw
  return loop_; // test
}

} /* namespace k */
