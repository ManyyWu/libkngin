#ifndef KNGIN_SYSTEM_ERROR_H
#define KNGIN_SYSTEM_ERROR_H

#include <string>
#include "core/base/error_code.h"
#include "core/base/define.h"

KNGIN_NAMESPACE_K_BEGIN

class system_error;

inline
std::string
system_error_str (const char *what, const error_code &ec);

inline
std::string
system_error_str (const error_code &ec);

class system_error : public std::exception {
public:
    system_error  () = delete;

    explicit
    system_error  (const char *what, error_code ec = k::last_error())
        : m_what(std::string("[k::system_error] ")
                 + system_error_str(what, ec)), m_ec(ec) {}

    explicit
    system_error  (const std::string &what, error_code ec = k::last_error())
        : m_what(std::string("[k::system_error] ")
                 + system_error_str(what.c_str(), ec)), m_ec(ec) {}

    explicit
    system_error  (std::string &&what, error_code ec = k::last_error())
        : m_what(std::string("[k::system_error] ")
                 + system_error_str(what.c_str(), ec)), m_ec(ec) {}

    system_error  (const k::system_error &e)
        : m_what(e.m_what), m_ec(e.m_ec) {}

    virtual
    ~system_error () = default;

public:
    k::system_error &
    operator =    (const k::system_error &e) = delete;

public:
    virtual const char *
    what          () const noexcept
    { return m_what.c_str(); }

    virtual const error_code
    code          () const noexcept
    { return m_ec; }

private:
    const std::string m_what;

    const error_code  m_ec;
};

inline
std::string
system_error_str (const char *what, const error_code &ec)
{
    return std::string(what ? what : "")
                       + " - "
                       + ec.message()
                       + ":"
                       + std::to_string(ec.value());
}

inline
std::string
system_error_str (const error_code &ec)
{
    return ec.message() + ":" + std::to_string(ec.value());
}

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_SYSTEM_ERROR_H */
