#include "kngin/core/base/memory.h"
#include "kngin/core/base/rmutex.h"
#include "detail/core/base/rmutex_impl.h"

namespace k {

rmutex::rmutex ()
 : impl_(nullptr) {
  impl_ = new rmutex_impl();
}

rmutex::~rmutex () noexcept {
  safe_release(impl_);
}

void
rmutex::lock () {
  impl_->lock();
}

void
rmutex::unlock () {
  impl_->unlock();
}

bool
rmutex::try_lock () {
  return impl_->try_lock();
}

} /* namespace k */
