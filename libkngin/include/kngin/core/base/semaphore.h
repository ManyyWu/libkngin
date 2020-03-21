#ifndef KNGIN_SEMAPHORE_H
#define KNGIN_SEMAPHORE_H

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/timestamp.h"
#include "kngin/core/base/detail.h"

KNGIN_NAMESPACE_K_BEGIN

class semaphore : public noncopyable {
public:
  explicit
  semaphore (int initval);

  ~semaphore () noexcept;

  bool
  wait ();

  bool
  timed_wait (timestamp ms);

  void
  post ();

private:
  semaphore_impl *impl_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_SEMAPHORE_H */