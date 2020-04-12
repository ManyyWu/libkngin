#include "kngin/core/base/memory.h"
#include "kngin/core/base/mutex.h"
#include "detail/core/base/mutex_impl.h"

namespace k {

mutex::mutex ()
 : impl_(nullptr) {
  impl_ = new mutex_impl();
}

mutex::~mutex () noexcept {
  safe_release(impl_);
}

void
mutex::lock () {
  impl_->lock();
}

void
mutex::unlock () {
  impl_->unlock();
}

bool
mutex::try_lock () {
  return impl_->try_lock();
}

} /* namespace k */
