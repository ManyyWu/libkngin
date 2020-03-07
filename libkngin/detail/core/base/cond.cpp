#include "kngin/core/base/memory.h"
#include "kngin/core/base/cond.h"
#include "detail/core/base/cond_impl.h"
#include "kngin/core/base/mutex.h"

KNGIN_NAMESPACE_K_BEGIN

cond::cond (mutex &mutex) noexcept
  : impl_(new cond_impl(*mutex.impl_)) {
}

cond::~cond () noexcept {
  safe_release(impl_);
}

void
cond::wait () noexcept {
  impl_->wait();
}

void
cond::timed_wait (timestamp ms) noexcept {
  impl_->timed_wait(ms);
}

void
cond::signal () noexcept {
  impl_->signal();
}

void
cond::broadcast() noexcept {
  impl_->broadcast();
}

KNGIN_NAMESPACE_K_END
