#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <string>
#include <exception>
#include "core/define.h"

KNGIN_NAMESPACE_K_BEGIN

class exception : public std::exception {
public:
    exception  () KNGIN_NOEXP = default;

    explicit
    exception  (const char *_what) KNGIN_EXP;

    virtual
    ~exception () KNGIN_NOEXP = default;

public:
    virtual const char *
    what       () const KNGIN_NOEXP { return m_what.c_str(); }

    const std::string &
    dump       () const KNGIN_EXP   { return m_dump_str; }

protected:
    void
    dump_stack () KNGIN_EXP;

private:
    const std::string m_what;

    std::string       m_dump_str;
};

KNGIN_NAMESPACE_K_END

#endif /* _EXCEPTION_H_ */
