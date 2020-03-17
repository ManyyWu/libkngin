#include "kngin/core/base/memory.h"
#include "kngin/core/base/rmutex.h"
#include "detail/core/base/rmutex_impl.h"

KNGIN_NAMESPACE_K_BEGIN

rmutex::rmutex ()
 : impl_(nullptr) {
  impl_ = new rmutex_impl();
}

rmutex::~rmutex () noexcept {
  safe_release(impl_);
}

void
rmutex::lock () noexcept {
  impl_->lock();
}

void
rmutex::unlock () noexcept {
  impl_->unlock();
}

bool
rmutex::try_lock () noexcept {
  return impl_->try_lock();
}

KNGIN_NAMESPACE_K_END
