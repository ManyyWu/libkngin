#ifndef KNGIN_NONCOPYABLE_H
#define KNGIN_NONCOPYABLE_H

#include "kngin/core/define.h"

namespace k {

class noncopyable {
protected:
  noncopyable  () = default;

  ~noncopyable () = default;

public:
  noncopyable  (const noncopyable &) = delete;

  noncopyable &
  operator =   (const noncopyable &) = delete;
};

} /* namespace k */

#endif /* KNGIN_NONCOPYABLE_H */
