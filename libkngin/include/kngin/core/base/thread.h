#ifndef KNGIN_THREAD_H
#define KNGIN_THREAD_H

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/timestamp.h"
#include "kngin/core/base/detail.h"
#include <functional>

KNGIN_NAMESPACE_K_BEGIN

class thread : public noncopyable {
public:
  typedef std::function<int (void)> thread_proc;
  typedef uint64_t tid_type;

  struct thread_opt {
    size_t stack_size;
  };

  explicit
  thread (thread_proc &&proc,
          const char *name = nullptr,
          thread_opt *opt = nullptr);

  ~thread () noexcept;

  int
  join () noexcept;

  bool
  joinable () const noexcept;

  const char *
  name () const noexcept;

  static
  tid_type
  tid () noexcept;

  static
  void
  sleep(timestamp ms) noexcept;

  bool
  equal_to (thread &thr) noexcept;

private:
  thread_impl *impl_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_THREAD_H */