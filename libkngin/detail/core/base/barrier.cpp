#include "kngin/core/base/barrier.h"
#include "detail/core/base/barrier_impl.h"

KNGIN_NAMESPACE_K_BEGIN

barrier::barrier (int count)
 : impl_(nullptr) {
  impl_ = new barrier_impl(count);
}

barrier::~barrier () noexcept {
  safe_release(impl_);
}

void
barrier::reinit (int count) noexcept {
  impl_->reinit(count);
}

bool
barrier::wait () noexcept {
  return impl_->wait();
}

void
barrier::destroy () noexcept {
  impl_->destroy();
}

bool
barrier::destroyed () const noexcept {
  return impl_->destroyed();
}

KNGIN_NAMESPACE_K_END
