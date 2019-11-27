#ifndef _SYSTEM_ERROR_H_
#define _SYSTEM_ERROR_H_

#ifdef _WIN32
#include <Windows.h>
#else
#include <cerrno>
#endif
#include <system_error>
#include "core/define.h"

KNGIN_NAMESPACE_K_BEGIN

inline std::error_code
int2ec           (int32_t _code) KNGIN_EXP;

inline std::error_code
last_error       () KNGIN_EXP;

inline std::string
system_error_str (const char *_what, std::error_code &_ec) KNGIN_EXP;

inline std::string
system_error_str (std::error_code &_ec) KNGIN_EXP;

class system_error {
public:
    system_error  () KNGIN_NOEXP = delete;

    explicit
    system_error  (const char *_what,
                   std::error_code _ec = k::last_error()) KNGIN_EXP
        : m_what(_what), m_ec(_ec)     {}

    virtual
    ~system_error () KNGIN_NOEXP = default;

    virtual const char *
    what          () const KNGIN_NOEXP { return m_what.c_str(); }

    virtual const std::error_code
    code          () const KNGIN_NOEXP { return m_ec; }

private:
    const std::string     m_what;

    const std::error_code m_ec;
};

inline std::error_code
int2ec (int32_t _code) KNGIN_EXP
{
    return std::make_error_code(static_cast<std::errc>(_code));
}

inline std::error_code
last_error () KNGIN_EXP
{
#ifdef _WIN32
    return std::make_error_code(static_cast<std::errc>(::GetLastError()));
#else
    return std::make_error_code(static_cast<std::errc>(errno));
#endif
}

inline std::string
system_error_str (const char *_what, std::error_code &_ec) KNGIN_EXP
{
    return std::string(_what) + _ec.message() + ":" + std::to_string(_ec.value());
}

inline std::string
system_error_str (std::error_code &_ec) KNGIN_EXP
{
    return _ec.message() + ":" + std::to_string(_ec.value());
}

KNGIN_NAMESPACE_K_END

#endif /* _SYSTEM_ERROR_H_ */
