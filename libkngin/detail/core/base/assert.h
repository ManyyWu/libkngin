#ifndef KNGIN_ASSERT_H
#define KNGIN_ASSERT_H

#include <cassert>

namespace k {

inline
void
kngin_assert (bool exp, const char *what = "") {
  if (!exp) {
    assert(exp);
    log_error("assert: %s", what);
  }
}

} /* namespace k */

#endif /* KNGIN_ASSERT_H */
