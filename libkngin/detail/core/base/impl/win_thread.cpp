#include "kngin/core/define.h"
#if defined(KNGIN_USE_WIN_THREAD)

#include "detail/core/base/impl/win_thread.h"
#include "kngin/core/base/system_error.h"
#include "kngin/core/base/log.h"
#include "kngin/core/base/memory.h"

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

void
win_thread::create_thread (thread_data *&data, thread::thread_opt *opt) {
  size_t stack_size = opt ? opt->stack_size : 0;
  thr_ = (handle_t)::_beginthreadex(NULL,
                                    static_cast<unsigned>(stack_size),
                                    win_thread::start,
                                    data,
                                    0,
                                    &tid_);
  if (!thr_)
    throw_system_error("::_beginthreadex() error", ERRNO(errno));
}

unsigned
win_thread::start (void *args) noexcept {
  unsigned code = 0;
  auto *data = static_cast<thread_data *>(args);

  log_debug("thread \"%s\" is running, tid = %" PRIu64, data->name.c_str(), thread::tid());
  try {
    if (data->thr_fn)
      code = data->thr_fn();
  } catch (const k::exception &e) {
    log_fatal("win_thread::start(), thread = \"%s\", message = %s",
          data->name.c_str(), e.what());
    log_fatal("%s", e.dump());
  } catch (const std::exception &e) {
    log_fatal("win_thread::start(), thread = \"%s\", message = %s",
          data->name.c_str(), e.what());
  } catch (...) {
    log_fatal("win_thread::start(), thread = \"%s\", message = unknown exception",
          data->name.c_str());
  }
  log_debug("thread \"%s\" stopped, tid = %" PRIu64, data->name.c_str(), thread::tid());

  safe_release(data);
  return code;
}

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_WIN_THREAD) */
