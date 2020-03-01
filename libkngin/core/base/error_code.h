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
        : m_code(0) {}

    error_code          (error_type code) noexcept
        : m_code(code) {}

    error_code          (const error_code &code) noexcept
        : m_code(code.m_code) {}

    error_code          (const error_code &&code) noexcept
        : m_code(code.m_code) {}

    ~error_code         () = default;

public:
    std::string
    message             () const
    { return format_error_str(m_code); }

    error_type
    value               () const noexcept
    { return m_code; }

public:
    explicit
    operator bool       () const noexcept
    { return m_code; }

    explicit
    operator error_type () const noexcept
    { return m_code; }

    error_code &
    operator =          (error_type code) noexcept
    { m_code = code; return *this; }

    error_code &
    operator =          (const error_code &code) noexcept
    { m_code = code.m_code; return *this; }

    error_code &
    operator =          (const error_code &&code) noexcept
    { m_code = code.m_code; return *this; }

public:
    bool
    operator ==         (const error_type code) const noexcept
    { return (m_code == code); }

    bool
    operator ==         (const error_code &code) const noexcept
    { return (m_code == code.m_code); }

    bool
    operator ==         (const error_code &&code) const noexcept
    { return (m_code == code.m_code); }

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
    error_type m_code;
};

inline
bool
operator == (int code1, const error_code &code2) noexcept
{ return (code1 == code2.m_code); }

inline
bool
operator == (int code1, const error_code &&code2) noexcept
{ return (code1 == code2.m_code); }

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_ERROR_CODE_H */