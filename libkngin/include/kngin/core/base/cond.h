#ifndef KNGIN_COND_H
#define KNGIN_COND_H

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/timestamp.h"
#include "kngin/core/base/detail.h"

KNGIN_NAMESPACE_K_BEGIN

class mutex;
class rmutex;
class cond : public noncopyable {
public:
  explicit
  cond (mutex &mutex);

  explicit
  cond (rmutex &mutex);

  ~cond () noexcept;

  void
  wait ();

  void
  timed_wait (timestamp ms);

  void
  signal ();

  void
  broadcast ();

private:
  cond_impl *impl_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_COND_H */
