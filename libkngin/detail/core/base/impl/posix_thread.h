#ifndef KNGIN_POSIX_THREAD_H
#define KNGIN_POSIX_THREAD_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_POSIX_THREAD)

#if defined(KNGIN_SYSTEM_WIN32)
#  include "pthreads_win32/pthread.h"
#else
#  include <pthread.h>
#endif /* defined(KNGIN_SYTEM_WIN32) */
#include "kngin/core/base/callback.h"

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class posix_thread {
public:
  typedef k::thread_proc thread_proc;

  union thread_error_code {
    void *ptr;
    int code;

    thread_error_code ()
     : ptr(0) {
    }
  };

  struct thread_data {
    std::string name;
    thread_proc thr_fn;
    thread_data **pdata;

    thread_data (const char *name, thread_proc &&thr_fn)
     : name(name),
       thr_fn(std::move(thr_fn)),
       pdata(nullptr) {
    }
  };

  explicit
  posix_thread (thread_data *data)
   : name_(data->name),
     pthr_(),
     joined_(false) {
    this->create_thread(data);
  }

  ~posix_thread () {
    if (joinable())
      this->detach();
  }

  int
  join () noexcept {
    thread_error_code code;
    assert(0 == ::pthread_join(pthr_, &code.ptr));
    joined_ = true;
    return code.code;
  }

  void
  detach () noexcept {
    assert(0 == ::pthread_detach(pthr_));
  }

  bool
  joinable () const noexcept {
    return !joined_;
  }

  const std::string &
  name () const noexcept {
    return name_;
  }

  bool
  equal_to (posix_thread &thr) noexcept {
    return ::pthread_equal(pthr_, thr.pthr_);
  }

private:
  static
  void *
  start (void *args) noexcept;

  void
  create_thread (thread_data *data);

private:
  const std::string name_;

  pthread_t pthr_;

  bool joined_;
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_POSIX_THREAD) */

#endif /* KNGIN_POSIX_THREAD_H */