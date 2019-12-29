#ifndef KNGIN_EXCEPTION_H
#define KNGIN_EXCEPTION_H

#include <string>
#include <exception>
#include "core/define.h"
#include "core/noncopyable.h"

KNGIN_NAMESPACE_K_BEGIN

class exception : public std::exception {
public:
    exception ()
        : m_what(), m_dump_str()
    { dump_stack(); }

    explicit
    exception (const char *_what)
        : m_what(std::string("[k::exception] ") + _what), m_dump_str()
    { dump_stack(); }

    explicit
    exception (const std::string &_what)
        : m_what(std::string("[k::exception] ") + _what), m_dump_str()
    { dump_stack(); }

    exception (const k::exception &_e)
        : m_what(_e.m_what), m_dump_str(_e.m_dump_str) {}


    virtual
    ~exception () = default;

public:
    k::exception &
    operator = (const k::exception &) = delete;

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
