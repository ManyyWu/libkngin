#ifndef KNGIN_SCOPED_FLAG_H
#define KNGIN_SCOPED_FLAG_H

#include "kngin/core/define.h"

KNGIN_NAMESPACE_K_BEGIN

template <typename Tp1, typename Tp2>
class scoped_flag {
public:
  scoped_flag (Tp1 &flag, Tp2 final) noexcept
   : flag_(flag),
     final_(final) {
  }

  ~scoped_flag () noexcept {
    flag_ = final_;
  }

private:
  Tp1 &flag_;
  Tp2 final_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_SCOPED_FLAG_H */