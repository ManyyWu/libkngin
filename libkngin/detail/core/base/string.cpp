#include "kngin/core/define.h"
#include "kngin/core/base/string.h"
#include "kngin/core/base/exception.h"
#include "kngin/core/base/common.h"
#include <cassert>
#include <cstdio>
#include <cstdarg>

KNGIN_NAMESPACE_K_BEGIN

std::string &
format_string(std::string &result, const char *fmt, va_list vl) {
  assert(fmt);

  va_list vl_copy;
  va_copy(vl_copy, vl);
  int length = vsnprintf(NULL, 0, fmt, vl_copy);
  va_end(vl_copy);
  if (length < 0)
    throw_exception("invalid format or parameters");

  if (length > 0) {
    result.resize(length + 1);
    va_copy(vl_copy, vl);
    vsnprintf(result.data(), length + 1, fmt, vl_copy);
    va_end(vl_copy);
    result[length] = '\0';
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

KNGIN_NAMESPACE_K_END
