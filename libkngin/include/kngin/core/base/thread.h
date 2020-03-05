#ifndef KNGIN_THREAD_H
#define KNGIN_THREAD_H

#include "kngin/core/define.h"
#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/timestamp.h"
#include "kngin/core/base/impl.h"
#include "kngin/core/base/callback.h"

TYPEDEF_THREAD_IMPL(thread_impl);

KNGIN_NAMESPACE_K_BEGIN

class thread : public noncopyable {
public:
  typedef k::thread_proc thread_proc;

  typedef uint64_t tid_type;

  explicit
  thread (thread_proc &&proc, const char *name = nullptr);

  ~thread () noexcept;

  int
  join () noexcept;

  bool
  joinable () const noexcept;

  const std::string &
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