#ifndef KNGIN_MEMORY_H
#define KNGIN_MEMORY_H

#include "kngin/core/define.h"
#include <cassert>

KNGIN_NAMESPACE_K_BEGIN

template <typename Tp>
inline
void
safe_release (Tp *&ptr) {
  assert(ptr);
  delete ptr;
  ptr = nullptr;
}

template <typename Tp>
inline
void
safe_release_array (Tp *&ptr) {
  assert(ptr);
  delete[] ptr;
  ptr = nullptr;
}

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_MEMORY_H */