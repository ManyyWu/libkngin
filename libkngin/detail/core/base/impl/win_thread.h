#ifndef KNGIN_WIN_THREAD_H
#define KNGIN_WIN_THREAD_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_WIN_THREAD)

#include "detail/core/base/win_utils.h"
#include <process.h>

namespace k::detail::impl {

class win_thread {
public:
  typedef thread::thread_proc thread_proc;

  struct thread_data {
    std::string name;
    thread_proc thr_fn;

    thread_data (const char *name, thread_proc &&thr_fn) noexcept
     : name(name),
       thr_fn(std::move(thr_fn)) {
    }
  };

  explicit
  win_thread (thread_data *&data, thread::thread_opt *opt)
   : name_(data->name),
     thr_(nullptr),
     joined_(false) {
    create_thread(data, opt);
  }

  ~win_thread () noexcept {
    if (joinable())
      detach();
  }

  int
  join () noexcept {
    assert(WAIT_OBJECT_0 == ::WaitForSingleObject(thr_, INFINITE));
    ::CloseHandle(thr_);
    thr_ = nullptr;
    joined_ = true;
    return 0;
  }

  void
  detach () noexcept {
    ::CloseHandle(thr_);
    thr_ = nullptr;
  }

  bool
  joinable () const noexcept {
    return !joined_;
  }

  const char *
  name () const noexcept {
    return name_.c_str();
  }

  bool
  equal_to (win_thread &thr) const noexcept {
    return (thr.tid_ == tid_);
  }

private:
  static
  unsigned
  start (void *args) noexcept;

  void
  create_thread (thread_data *&data, thread::thread_opt *opt);

private:
  const std::string name_;

  handle_t thr_;

  unsigned tid_;

  std::atomic_bool joined_;
};

} /* namespace k::detail::impl */

#endif /* defined(KNGIN_USE_WIN_THREAD) */

#endif /* KNGIN_WIN_THREAD_H */