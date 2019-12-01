#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <string>
#include <exception>
#include "core/define.h"

KNGIN_NAMESPACE_K_BEGIN

class exception : public std::exception {
public:
    exception  () = default;

    explicit
    exception  (const char *_what);

    explicit
    exception  (const std::string &_what);

    explicit
    exception  (std::string &&_what);

    virtual
    ~exception () = default;

public:
    virtual const char *
    what       () const KNGIN_NOEXP
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

#endif /* _EXCEPTION_H_ */
