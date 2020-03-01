#ifndef KNGIN_ERROR_CODE_H
#define KNGIN_ERROR_CODE_H

#include <cerrno>
#include <cstring>
#include <string>
#include "core/base/define.h"

KNGIN_NAMESPACE_K_BEGIN

#ifdef _WIN32
typedef int64_t error_type;
#else
typedef int     error_type;
#endif

#ifdef _WIN32
#define CERR(code) (assert((code) > 0), -(code))
#else
#define CERR(code) (code)
#endif

inline
error_type
last_error ()
{
#ifdef _WIN32
    return ::GetLastError();
#else
    return (errno);
#endif
}

class error_code {
public:
    error_code          () noexcept
        : code_(0) {}

    error_code          (error_type code) noexcept
        : code_(code) {}

    error_code          (const error_code &code) noexcept
        : code_(code.code_) {}

    error_code          (const error_code &&code) noexcept
        : code_(code.code_) {}

    ~error_code         () = default;

public:
    std::string
    message             () const
    { return format_error_str(code_); }

    error_type
    value               () const noexcept
    { return code_; }

public:
    explicit
    operator bool       () const noexcept
    { return code_; }

    explicit
    operator error_type () const noexcept
    { return code_; }

    error_code &
    operator =          (error_type code) noexcept
    { code_ = code; return *this; }

    error_code &
    operator =          (const error_code &code) noexcept
    { code_ = code.code_; return *this; }

    error_code &
    operator =          (const error_code &&code) noexcept
    { code_ = code.code_; return *this; }

public:
    bool
    operator ==         (const error_type code) const noexcept
    { return (code_ == code); }

    bool
    operator ==         (const error_code &code) const noexcept
    { return (code_ == code.code_); }

    bool
    operator ==         (const error_code &&code) const noexcept
    { return (code_ == code.code_); }

    friend
    bool
    operator ==         (int code1, const error_code &code2) noexcept;

    friend
    bool
    operator ==         (int code1, const error_code &&code2) noexcept;

private:
    static std::string
    format_error_str    (error_type code);

private:
    error_type code_;
};

inline
bool
operator == (int code1, const error_code &code2) noexcept
{ return (code1 == code2.code_); }

inline
bool
operator == (int code1, const error_code &&code2) noexcept
{ return (code1 == code2.code_); }

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_ERROR_CODE_H */