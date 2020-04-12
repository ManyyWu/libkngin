#ifndef KNGIN_MEMORY_H
#define KNGIN_MEMORY_H

#include "kngin/core/define.h"
#include <cassert>

namespace k {

template <typename Tp>
inline
void
safe_release (Tp *&ptr) {
  delete ptr;
  ptr = nullptr;
}

template <typename Tp>
inline
void
safe_release_array (Tp *&ptr) {
  delete[] ptr;
  ptr = nullptr;
}

} /* namespace k */

#endif /* KNGIN_MEMORY_H */