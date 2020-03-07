#ifndef KNGIN_COND_H
#define KNGIN_COND_H

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/timestamp.h"
#include "kngin/core/base/impl.h"

TYPEDEF_COND_IMPL(cond_impl);

KNGIN_NAMESPACE_K_BEGIN

class mutex;
class cond : public noncopyable {
public:
  explicit
  cond (mutex &mutex) noexcept;

  ~cond () noexcept;

  void
  wait () noexcept;

  void
  timed_wait (timestamp ms) noexcept;

  void
  signal () noexcept;

  void
  broadcast () noexcept;

private:
  cond_impl *impl_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_COND_H */
