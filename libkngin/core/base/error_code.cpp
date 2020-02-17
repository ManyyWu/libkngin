#include "core/base/error_code.h"

KNGIN_NAMESPACE_K_BEGIN

std::string
error_code::format_error_str (error_type _code)
{
#ifdef _WIN32
    std::string _result;
    char*       _buf = NULL;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                   FORMAT_MESSAGE_IGNORE_INSERTS, NULL, _code,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&_buf, 0, NULL);
    if (_buf)
        _result = _buf;
    else
        _result = "Unknown error";
    if (_buf)
        LocalFree(_buf);
    if (_result.size() > 3)
        _result.resize(_result.size() - 2); // remove \r\n
    return _result;
#else
    return std::string(::strerror(errno));
#endif
}

KNGIN_NAMESPACE_K_END
