#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <string>
#include <exception>
#include "define.h"

#define KNGIN_EXP   noexcept(false)
#define KNGIN_NOEXP noexcept(true)

KNGIN_NAMESPACE_K_BEGIN

class exception : public std::exception {
public:
    exception  () = default;

    explicit
    exception  (const char *_what) KNGIN_EXP;

    virtual
    ~exception () = default;

public:
    virtual const char *
    what       () const KNGIN_NOEXP
    { return m_what.c_str(); }

    const std::string &
    dump       () const KNGIN_EXP
    { return m_dump_str; }

protected:
    void
    dump_stack () KNGIN_EXP;

protected:
    const std::string m_what;

    std::string       m_dump_str;
};

KNGIN_NAMESPACE_K_END

#endif /* _EXCEPTION_H_ */
