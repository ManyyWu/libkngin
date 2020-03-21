#include "kngin/core/base/memory.h"
#include "kngin/core/base/cond.h"
#include "detail/core/base/cond_impl.h"
#include "kngin/core/base/mutex.h"
#include "kngin/core/base/rmutex.h"

KNGIN_NAMESPACE_K_BEGIN

cond::cond (mutex &mutex)
 : impl_(nullptr) {
  impl_ = new cond_impl(*mutex.impl_);
}

cond::cond (rmutex &mutex)
 : impl_(nullptr) {
  impl_ = new cond_impl(*mutex.impl_);
}

cond::~cond () noexcept {
  safe_release(impl_);
}

void
cond::wait () {
  impl_->wait();
}

void
cond::timed_wait (timestamp ms) {
  impl_->timed_wait(ms);
}

void
cond::signal () {
  impl_->signal();
}

void
cond::broadcast() {
  impl_->broadcast();
}

KNGIN_NAMESPACE_K_END
