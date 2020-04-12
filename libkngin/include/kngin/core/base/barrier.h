#ifndef KNGIN_BARRIER_H
#define KNGIN_BARRIER_H

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/detail.h"

namespace k {

class barrier : public noncopyable {
public:
  barrier (unsigned count);

  ~barrier () noexcept;

  void
  reinit (unsigned count);

  bool
  wait ();

  void
  destroy ();

  bool
  destroyed () const noexcept;

private:
  barrier_impl *impl_;
};

} /* namespace k */

#endif /* KNGIN_BARRIER_H */