#include "kngin/core/base/error_code.h"
#if defined(KNGIN_SYSTEM_WIN32)
#include "detail/core/base/win_utils.h"
#endif /* defined(KNGIN_SYSTEM_WIN32) */

namespace k {

errno_type
last_error () {
#if defined(KNGIN_SYSTEM_WIN32)
  return trans_sys_error(::GetLastError());
#else
  return KNGIN_ERRNO(errno);
#endif /* defined(KNGIN_SYSTEM_WIN32) */
}

//std::string
//error_code::get_error_str (errno_type code) {
//#if defined(KNGIN_SYSTEM_WIN32)
//  if (code > 0) {
//    std::string result;
//    char* buf = nullptr;
//    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
//                   FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, DWORD(code),
//                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buf, 0, nullptr);
//    result = buf ? buf : "unknown system error";
//    if (buf)
//        LocalFree(buf);
//    if (result.size() > 3)
//        result.resize(result.size() - 2); // remove \r\n
//    return result;
//  }else {
//    auto *str = ::strerror(static_cast<int>(-code));
//    return str ? str : "unknown system error";
//  }
//#else
//  auto *str = ::strerror(static_cast<int>(code));
//  return str ? str : "unknown system error";
//#endif /* defined(KNGIN_SYSTEM_WIN32) */
//}

} /* namespace k */
