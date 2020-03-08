#ifndef KNGIN_BARRIER_H
#define KNGIN_BARRIER_H

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/detail.h"

KNGIN_NAMESPACE_K_BEGIN

class barrier : public noncopyable {
public:
  barrier (int count);

  ~barrier () noexcept;

  void
  reinit (int count) noexcept;

  bool
  wait () noexcept;

  void
  destroy () noexcept;

  bool
  destroyed () const noexcept;

private:
  barrier_impl *impl_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_BARRIER_H */