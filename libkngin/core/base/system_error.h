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
        : what_(std::string("[k::system_error] ")
                 + system_error_str(what, ec)), ec_(ec) {}

    explicit
    system_error  (const std::string &what, error_code ec = k::last_error())
        : what_(std::string("[k::system_error] ")
                 + system_error_str(what.c_str(), ec)), ec_(ec) {}

    explicit
    system_error  (std::string &&what, error_code ec = k::last_error())
        : what_(std::string("[k::system_error] ")
                 + system_error_str(what.c_str(), ec)), ec_(ec) {}

    system_error  (const k::system_error &e)
        : what_(e.what_), ec_(e.ec_) {}

    virtual
    ~system_error () = default;

public:
    k::system_error &
    operator =    (const k::system_error &e) = delete;

public:
    virtual const char *
    what          () const noexcept
    { return what_.c_str(); }

    virtual const error_code
    code          () const noexcept
    { return ec_; }

private:
    const std::string what_;

    const error_code  ec_;
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
