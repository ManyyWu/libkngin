#include "kngin/core/define.h"
#include "kngin/core/base/string.h"
#include <cassert>
#include <cstdio>
#include <cstdarg>

KNGIN_NAMESPACE_K_BEGIN

#if defined(KNGIN_SYSTEM_WIN32)
static
int
vasprintf (char **ptr,
           const char *, va_list arg) {
  return 0;
}
#endif /* defined(KNGIN_SYSTEM_WIN32) */

std::string
make_string (const char *fmt, ...) {
  assert(fmt);
  va_list vl;
  va_start(vl, fmt);
  char *buf = nullptr;
  vasprintf(&buf, fmt, vl);
  va_end(vl);
  return (buf ? buf : "");
}

KNGIN_NAMESPACE_K_END
