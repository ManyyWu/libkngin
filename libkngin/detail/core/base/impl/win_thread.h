#ifndef KNGIN_WIN_THREAD_H
#define KNGIN_WIN_THREAD_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_WIN_THREAD)

#include "detail/core/base/win_utils.h"
#include <process.h>

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

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
     hthr_(nullptr),
     joined_(false) {
    this->create_thread(data, opt);
  }

  ~win_thread () noexcept {
    if (joinable())
      this->detach();
  }

  int
  join () noexcept {
    assert(WAIT_OBJECT_0 == ::WaitForSingleObject(hthr_, INFINITE));
    ::CloseHandle(hthr_);
    joined_ = true;
    return 0;
  }

  void
  detach () noexcept {
    ::CloseHandle(hthr_);
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

  HANDLE hthr_;

  unsigned tid_;

  std::atomic_bool joined_;
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_WIN_THREAD) */

#endif /* KNGIN_WIN_THREAD_H */