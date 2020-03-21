#include "detail/core/base/impl/posix_thread.h"
#include "kngin/core/base/system_error.h"
#include "kngin/core/base/log.h"
#include "kngin/core/base/memory.h"

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

void
posix_thread::create_thread (thread_data *data) {
  auto ec = ::pthread_create(&pthr_, nullptr, posix_thread::start, data);
  if (ec) {
    safe_release(data);
    throw_system_error("::pthread_create() error ", ERRNO(ec));
  }
}

void *
posix_thread::start (void *args) noexcept {
  thread_error_code code;
  auto *data = static_cast<thread_data *>(args);

  //debug("thread \"%s\" is running, tid = %" PRIu64, data->name.c_str(), thread::tid());
  try {
    if (data->thr_fn)
      code.code = data->thr_fn();
  } catch (const k::exception &e) {
    fatal("posix_thread::start(), thread = \"%s\", message = %s",
          data->name.c_str(), e.what());
    fatal("%s", e.dump());
  } catch (const std::exception &e) {
    fatal("posix_thread::start(), thread = \"%s\", message = %s",
          data->name.c_str(), e.what());
  } catch (...) {
    fatal("posix_thread::start(), thread = \"%s\", message = unknown exception",
          data->name.c_str());
  }
  //debug("thread \"%s\" stopped, tid = %" PRIu64, data->name.c_str(), thread::tid());

  safe_release(data);
  return code.ptr;
}

KNGIN_NAMESPACE_K_DETAIL_IMPL_END
