#include "kngin/net/service.h"

namespace k {

service::service ()
 : thread_([this] () -> int { loop_.run(); }),
   loop_() {

}

service::~service () noexcept {

}

void
service::run (size_t thread_num, crash_handler &&handler) {

}

void
service::stop () {

}

event_loop &
service::next_loop () noexcept {
  // throw
  return loop_;
}

} /* namespace k */
