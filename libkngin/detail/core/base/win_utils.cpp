#include "detail/core/base/win_utils.h"

#if defined(KNGIN_SYSTEM_WIN32)

namespace {
#if defined(KNGIN_SYSTEM_WIN32)
const LONGLONG g_monotonic_base = detail::get_monotonic_base();
#endif /* defined(KNGIN_SYSTEM_WIN32) */
}

namespace k::detail {

#  if !defined(KNGIN_USE_WINDOWS_GETTICKCOUNT)
LONGLONG
get_monotonic_base () {
  LARGE_INTEGER freq = {0};
  assert(TRUE == QueryPerformanceFrequency(&freq));
  return freq.QuadPart / 1000;
}
#  endif /* !defined(KNGIN_USE_WINDOWS_GETTICKCOUNT) */

#if defined(KNGIN_USE_IOCP_REACTOR)
bool g_have_get_iocp_status_ex = true;
#endif /* defined(KNGIN_USE_IOCP_REACTOR) */

} /* k::detail */

#endif /* defined(KNGIN_SYSTEM_WIN32) */
