#include "kngin/core/define.h"
#if defined(KNGIN_USE_POSIX_THREAD)

#include "detail/core/base/impl/posix_thread.h"
#include "kngin/core/base/system_error.h"
#include "kngin/core/base/log.h"
#include "kngin/core/base/memory.h"

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

void
posix_thread::create_thread (thread_data *&data, thread::thread_opt *opt) {
  auto ec = 0;
  pthread_attr_t *attr = nullptr;
  size_t stack_size = opt ? opt->stack_size : 0;
#if defined(_POSIX_THREAD_ATTR_STACKSIZE)
  if (stack_size) {
    attr = new pthread_attr_t;
    ec = ::pthread_attr_init(attr);
    if (ec) {
      safe_release(attr);
      throw_system_error("::pthread_attr_init() error", ec);
    }
    ::pthread_attr_setstacksize(attr, stack_size);
    size_t size;
    ::pthread_attr_getstacksize(attr, &size);
    log_info("thread \"%s\", set stack size: %" PRIu64, data->name.c_str(), size);
  }
#endif /* defined(_POSIX_THREAD_ATTR_STACKSIZE) */
  ec = ::pthread_create(&pthr_, attr, posix_thread::start, data);
  if (attr) {
    ::pthread_attr_destroy(attr);
    safe_release(attr);
  }
  if (ec)
    throw_system_error("::pthread_create() error ", ERRNO(ec));
}

void *
posix_thread::start (void *args) noexcept {
  thread_error_code code;
  auto *data = static_cast<thread_data *>(args);

  log_debug("thread \"%s\" is running, tid = %" PRIu64, data->name.c_str(), thread::tid());
  try {
    if (data->thr_fn)
      code.code = data->thr_fn();
  } catch (const k::exception &e) {
    log_fatal("posix_thread::start(), thread = \"%s\", message = %s",
          data->name.c_str(), e.what());
    log_fatal("%s", e.dump());
  } catch (const std::exception &e) {
    log_fatal("posix_thread::start(), thread = \"%s\", message = %s",
          data->name.c_str(), e.what());
  } catch (...) {
    log_fatal("posix_thread::start(), thread = \"%s\", message = unknown exception",
          data->name.c_str());
  }
  log_debug("thread \"%s\" stopped, tid = %" PRIu64, data->name.c_str(), thread::tid());

  safe_release(data);
  return code.ptr;
}

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_POSIX_THREAD) */
