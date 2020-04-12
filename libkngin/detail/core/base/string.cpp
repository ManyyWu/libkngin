#include "kngin/core/base/string.h"
#include "kngin/core/base/exception.h"
#include <cassert>
#include <cstdio>
#include <cstdarg>

namespace k {

std::string &
format_string(std::string &result, const char *fmt, va_list vl,
              size_t offset /* = 0 */, size_t extra /* = 0 */) {
  assert(fmt);

  va_list vl_copy;
  va_copy(vl_copy, vl);
  int length = vsnprintf(nullptr, 0, fmt, vl_copy);
  va_end(vl_copy);
  if (length < 0)
    throw_exception("invalid format or parameters");

  if (length > 0) {
    if (result.size() <= length + offset + extra)
      result.resize(length + offset + extra + 1);
    va_copy(vl_copy, vl);
    vsnprintf(result.data() + offset, length + 1, fmt, vl_copy);
    va_end(vl_copy);
    result[length + offset + extra] = '\0';
  }

  return result;
}
//#endif /* defined(KNGIN_SYSTEM_WIN32) */

std::string &
format_string (std::string &result, const char *fmt, ...) {
  assert(fmt);

  va_list vl;
  va_start(vl, fmt);
  format_string(result, fmt, vl);
  va_end(vl);

  return result;
}

std::string
format_string (const char *fmt, ...) {
  assert(fmt);
  std::string result;

  va_list vl;
  va_start(vl, fmt);
  format_string(result, fmt, vl);
  va_end(vl);

  return result;
}

} /* namespace k */
