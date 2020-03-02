#include "kngin/core/base/mutex.h"
#include "detail/core/base/mutex_impl.h"

KNGIN_NAMESPACE_K_BEGIN

mutex::mutex ()
 : impl_(new impl_type()) {
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
  impl_->try_lock();
}

KNGIN_NAMESPACE_K_END
