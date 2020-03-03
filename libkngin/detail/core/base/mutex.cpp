#include "kngin/core/base/memory.h"
#include "kngin/core/base/mutex.h"
#include "detail/core/base/mutex_impl.h"

KNGIN_NAMESPACE_K_BEGIN

mutex::mutex () noexcept
  : impl_(new mutex_impl()) {
}

mutex::~mutex () noexcept {
  safe_release(impl_);
}

void
mutex::lock () noexcept {
  impl_->lock();
}

void
mutex::unlock () noexcept {
  impl_->unlock();
}

bool
mutex::try_lock () noexcept {
  return impl_->try_lock();
}

KNGIN_NAMESPACE_K_END
