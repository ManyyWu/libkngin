#ifndef KNGIN_EXCEPTION_H
#define KNGIN_EXCEPTION_H

#include <string>
#include <exception>
#include "core/define.h"
#include "core/noncopyable.h"

KNGIN_NAMESPACE_K_BEGIN

class exception : public std::exception {
public:
    exception  ();

    explicit
    exception  (const char *_what);

    explicit
    exception  (const std::string &_what);

    exception  (std::string &&_what);

    virtual
    ~exception () = default;

public:
    virtual const char *
    what       () const KNGIN_NOEXCP
    { return m_what.c_str(); }

    const std::string &
    dump       () const
    { return m_dump_str; }

protected:
    void
    dump_stack ();

private:
    const std::string m_what;

    std::string       m_dump_str;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_EXCEPTION_H */
