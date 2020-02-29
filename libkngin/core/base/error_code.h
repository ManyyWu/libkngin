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
    error_code          () KNGIN_NOEXCP
        : m_code(0) {}

    error_code          (error_type _code) KNGIN_NOEXCP
        : m_code(_code) {}

    error_code          (const error_code &_code) KNGIN_NOEXCP
        : m_code(_code.m_code) {}

    error_code          (const error_code &&_code) KNGIN_NOEXCP
        : m_code(_code.m_code) {}

    ~error_code         () = default;

public:
    std::string
    message             () const
    { return format_error_str(m_code); }

    error_type
    value               () const KNGIN_NOEXCP
    { return m_code; }

public:
    explicit
    operator bool       () const KNGIN_NOEXCP
    { return m_code; }

    explicit
    operator error_type () const KNGIN_NOEXCP
    { return m_code; }

    error_code &
    operator =          (error_type _code) KNGIN_NOEXCP
    { m_code = _code; return *this; }

    error_code &
    operator =          (const error_code &_code) KNGIN_NOEXCP
    { m_code = _code.m_code; return *this; }

    error_code &
    operator =          (const error_code &&_code) KNGIN_NOEXCP
    { m_code = _code.m_code; return *this; }

public:
    bool
    operator ==         (const error_type _code) const KNGIN_NOEXCP
    { return (m_code == _code); }

    bool
    operator ==         (const error_code &_code) const KNGIN_NOEXCP
    { return (m_code == _code.m_code); }

    bool
    operator ==         (const error_code &&_code) const KNGIN_NOEXCP
    { return (m_code == _code.m_code); }

    friend
    bool
    operator ==         (int _code1, const error_code &_code2) KNGIN_NOEXCP;

    friend
    bool
    operator ==         (int _code1, const error_code &&_code2) KNGIN_NOEXCP;

private:
    static std::string
    format_error_str    (error_type _code);

private:
    error_type m_code;
};

inline
bool
operator == (int _code1, const error_code &_code2) KNGIN_NOEXCP
{ return (_code1 == _code2.m_code); }

inline
bool
operator == (int _code1, const error_code &&_code2) KNGIN_NOEXCP
{ return (_code1 == _code2.m_code); }

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_ERROR_CODE_H */