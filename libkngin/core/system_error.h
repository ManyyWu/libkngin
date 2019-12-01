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

class system_error;

inline std::error_code
int2ec           (int32_t _code);

inline std::error_code
last_error       ();

inline std::string
system_error_str (const char *_what, const std::error_code &_ec);

inline std::string
system_error_str (const std::error_code &_ec);

class system_error : public std::exception {
public:
    system_error  () = default;

    explicit
    system_error  (const char *_what, std::error_code _ec = k::last_error())
        : m_what(std::string("k::system_error() ")
                 + system_error_str(_what, _ec)), m_ec(_ec) {}

    explicit
    system_error  (const std::string &_what, std::error_code _ec = k::last_error())
        : m_what(std::string("k::system_error() ")
                 + system_error_str(_what.c_str(), _ec)), m_ec(_ec) {}

    explicit
    system_error  (std::string &&_what, std::error_code _ec = k::last_error())
        : m_what(std::string("k::system_error() ")
                 + system_error_str(_what.c_str(), _ec)), m_ec(_ec) {}

    virtual
    ~system_error () = default;

public:
    virtual const char *
    what          () const KNGIN_NOEXP
    { return m_what.c_str(); }

    virtual const std::error_code
    code          () const KNGIN_NOEXP
    { return m_ec; }

private:
    const std::string     m_what;

    const std::error_code m_ec;
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

inline std::string
system_error_str (const char *_what, const std::error_code &_ec)
{
    return std::string(_what ? _what : "")
                       + " - "
                       + _ec.message()
                       + ":"
                       + std::to_string(_ec.value());
}

inline std::string
system_error_str (const std::error_code &_ec)
{
    return _ec.message() + ":" + std::to_string(_ec.value());
}

KNGIN_NAMESPACE_K_END

#endif /* _SYSTEM_ERROR_H_ */
