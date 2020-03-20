#ifndef KNGIN_POSIX_THREAD_H
#define KNGIN_POSIX_THREAD_H

#include "kngin/core/define.h"
#if defined(KNGIN_USE_POSIX_THREAD)

#include <pthread.h>

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class posix_thread {
public:
  typedef thread::thread_proc thread_proc;

  union thread_error_code {
    void *ptr;
    int code;

    thread_error_code () noexcept
     : ptr(0) {
    }
  };

  struct thread_data {
    std::string name;
    thread_proc thr_fn;

    thread_data (const char *name, thread_proc &&thr_fn) noexcept
     : name(name),
       thr_fn(std::move(thr_fn)) {
    }
  };

  explicit
  posix_thread (thread_data *data)
   : name_(data->name),
     pthr_(),
     joined_(false) {
    this->create_thread(data);
  }

  ~posix_thread () noexcept {
    if (joinable())
      this->detach();
  }

  int
  join () noexcept {
    thread_error_code code;
    ::pthread_join(pthr_, &code.ptr);
    joined_ = true;
    return code.code;
  }

  void
  detach () noexcept {
    ::pthread_detach(pthr_);
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
  equal_to (posix_thread &thr) const noexcept {
    return !::pthread_equal(pthr_, thr.pthr_);
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

  std::atomic_bool joined_;
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* defined(KNGIN_USE_POSIX_THREAD) */

#endif /* KNGIN_POSIX_THREAD_H */