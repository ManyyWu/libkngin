#include "core/base/error_code.h"

KNGIN_NAMESPACE_K_BEGIN

std::string
error_code::format_error_str (error_type _code)
{
#ifdef _WIN32
    if (_code > 0) {
        std::string _result;
        char*       _buf = NULL;
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                       FORMAT_MESSAGE_IGNORE_INSERTS, NULL, DWORD(_code),
                       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&_buf, 0, NULL);
        _result = _buf ? _buf : "Unknown error";
        if (_buf)
            LocalFree(_buf);
        if (_result.size() > 3)
            _result.resize(_result.size() - 2); // remove \r\n
        return _result;
    } else {
        const char *_str = ::strerror(int(-_code));
        return _str ? _str : "Unknown error";
    }
#else
    const char * _str = std::string(::strerror(errno));
    return _str ? _str : "Unknown error";
#endif
}

KNGIN_NAMESPACE_K_END
