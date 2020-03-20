#include "detail/core/base/win_utils.h"

KNGIN_NAMESPACE_GROBLE_BEGIN
const LONGLONG g_monotonic_base = detail::get_monotonic_base();
KNGIN_NAMESPACE_GROBLE_END

KNGIN_NAMESPACE_DETAIL_BEGIN

#if !defined(KNGIN_USE_WINDOWS_GETTICKCOUNT)
LONGLONG
get_monotonic_base () {
  LARGE_INTEGER freq = {0};
  assert(TRUE == QueryPerformanceFrequency(&freq));
  return freq.QuadPart / 1000;
}
#endif /* !defined(KNGIN_USE_WINDOWS_GETTICKCOUNT) */

KNGIN_NAMESPACE_DETAIL_END
