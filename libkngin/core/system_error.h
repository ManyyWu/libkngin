#ifndef _SYSTEM_ERROR_H_
#define _SYSTEM_ERROR_H_

#ifdef _WIN32
#include <Windows.h>
#else
#include <cerrno>
#endif
#include <system_error>
#include "define.h"

KNGIN_NAMESPACE_K_BEGIN

inline std::error_code
int2ec     (int32_t _code);

inline std::error_code
last_error ();

class system_error : public std::system_error {
public:
    system_error  () = delete;

    explicit
    system_error  (const char *_what, std::error_code _ec = k::last_error())
        : std::system_error(_ec, _what) {}

    virtual
    ~system_error () = default;
};

inline std::error_code
int2ec (int32_t _code)
{
    return std::make_error_code(static_cast<std::errc>(_code));
}

inline std::error_code
last_error ()
{
#ifdef _WIN32
    return std::make_error_code(static_cast<std::errc>(::GetLastError()));
#else
    return std::make_error_code(static_cast<std::errc>(errno));
#endif
}

KNGIN_NAMESPACE_K_END

#endif /* _SYSTEM_ERROR_H_ */
