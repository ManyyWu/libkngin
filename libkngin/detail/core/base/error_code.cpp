#include "kngin/core/base/error_code.h"
#if defined(KNGIN_SYSTEM_WIN32)
#include "detail/core/base/win_utils.h"
#endif /* defined(KNGIN_SYSTEM_WIN32) */

KNGIN_NAMESPACE_K_BEGIN

error_type
last_error () {
#if defined(KNGIN_SYSTEM_WIN32)
  return ::GetLastError();
#else
  return (errno);
#endif /* defined(KNGIN_SYSTEM_WIN32) */
}

void
set_last_error (error_type ec) {
#if defined(KNGIN_SYSTEM_WIN32)
  if (ec < 0)
    errno = static_cast<int>(ERRNO(ec));
  else
    ::SetLastError(static_cast<int>(ec));
#else
  assert(ec >= 0);
  errno = ec;
#endif /* defined(KNGIN_SYSTEM_WIN32) */
}

std::string
error_code::get_error_str (error_type code) {
#if defined(KNGIN_SYSTEM_WIN32)
  if (code > 0) {
    std::string result;
    char* buf = nullptr;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                   FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, DWORD(code),
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buf, 0, nullptr);
    result = buf ? buf : "Unknown error";
    if (buf)
        LocalFree(buf);
    if (result.size() > 3)
        result.resize(result.size() - 2); // remove \r\n
    return result;
  }else {
    auto *str = ::strerror(static_cast<int>(-code));
    return str ? str : "Unknown error";
  }
#else
  auto *str = ::strerror(static_cast<int>(code));
  return str ? str : "Unknown error";
#endif /* defined(KNGIN_SYSTEM_WIN32) */
}

KNGIN_NAMESPACE_K_END
