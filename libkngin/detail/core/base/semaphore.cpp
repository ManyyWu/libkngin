#include "kngin/core/base/semaphore.h"
#include "detail/core/base/semaphore_impl.h"

namespace k {

semaphore::semaphore (unsigned initval)
 : impl_(nullptr) {
  impl_ = new semaphore_impl(initval);
}

semaphore::~semaphore () noexcept {
  safe_release(impl_);
}

bool
semaphore::wait () {
  return impl_->wait();
}

bool
semaphore::timed_wait (timestamp ms) {
  return impl_->timed_wait(ms);
}

void
semaphore::post () {
  impl_->post();
}

} /* namespace k */
