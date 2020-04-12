#include "kngin/core/base/barrier.h"
#include "detail/core/base/barrier_impl.h"

namespace k {

barrier::barrier (unsigned count)
 : impl_(nullptr) {
  impl_ = new barrier_impl(count);
}

barrier::~barrier () noexcept {
  safe_release(impl_);
}

void
barrier::reinit (unsigned count) {
  impl_->reinit(count);
}

bool
barrier::wait () {
  return impl_->wait();
}

void
barrier::destroy () {
  impl_->destroy();
}

bool
barrier::destroyed () const noexcept {
  return impl_->destroyed();
}

} /* namespace k */
