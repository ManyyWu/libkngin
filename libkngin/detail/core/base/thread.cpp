#include "kngin/core/base/memory.h"
#include "kngin/core/base/thread.h"
#include "detail/core/base/thread_impl.h"
#if !defined(KNGIN_SYSTEM_WIN32)
#  include <unistd.h>
#  include <sys/syscall.h>
#endif /* !defined(KNGIN_SYSTEM_WIN32) */

KNGIN_NAMESPACE_K_BEGIN

thread::thread (thread_proc &&proc, const char *name /* = nullptr */)
  : impl_(nullptr) {
  thread_impl::thread_data *data = nullptr;
  try {
    impl_ = new thread_impl(
              (data = new thread_impl::thread_data(name ? name : "",
                                                   std::move(proc))));
  } catch (...) {
    safe_release(data);
    safe_release(impl_);
  }
}

thread::~thread () noexcept {
  safe_release(impl_);
}

int
thread::join () noexcept {
  return impl_->join();
}

bool
thread::joinable () const noexcept {
  return impl_->joinable();
}

const char *
thread::name () const noexcept {
  return impl_->name();
}

thread::tid_type
thread::tid () noexcept {
#if defined(KNGIN_SYSTEM_WIN32)
  return ::GetCurrentThreadId();
#else
  return ::syscall(SYS_gettid);
#endif /* defined(KNGIN_SYSTEM_WIN32) */
}

void
thread::sleep(timestamp ms) noexcept {
#if defined(KNGIN_SYSTEM_WIN32)
  ::Sleep(static_cast<DWORD>(ms.value_uint()));
#else
  ::usleep(ms.value() * 1000);
#endif /* defined(KNGIN_SYSTEM_WIN32) */
}

bool
thread::equal_to (thread &thr) noexcept {
  return impl_->equal_to(*thr.impl_);
}

KNGIN_NAMESPACE_K_END
