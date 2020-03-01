#include <cassert>
#include "kngin/core/base/error_code.h"

KNGIN_NAMESPACE_K_BEGIN

std::string
error_code::get_error_str (error_type code) {
#if defined(KNGIN_SYSTEM_WIN32)
  if (code > 0) {
    std::string result;
    char* buf = NULL;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                   FORMAT_MESSAGE_IGNORE_INSERTS, NULL, DWORD(code),
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buf, 0, NULL);
    result = buf ? buf : "Unknown error";
    if (buf)
        LocalFree(buf);
    if (result.size() > 3)
        result.resize(result.size() - 2); // remove \r\n
    return result;
  }else {
    const char *str = ::strerror(int(-code));
    return str ? str : "Unknown error";
  }
#else
  assert(code > 0);
  const char *str = ::strerror(int(code));
  return str ? str : "Unknown error";
#endif /* defined(KNGIN_SYSTEM_WIN32) */
}

KNGIN_NAMESPACE_K_END
