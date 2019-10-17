#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <string>
#include <exception>
#include <stdexcept>
#include "define.h"

#define __EXP   noexcept(false)
#define __NOEXP noexcept(true)

__NAMESPACE_BEGIN

class exception {
public:
    exception  () = delete;

public:
    explicit
    exception  (const char *_what) : m_str(_what) { this->dump_stack(); }

    ~exception () = default;

public:
    const std::string &
    what       () const { return m_str; }

    const std::string &
    dump       () const { return m_dump_str; }

protected:
    void
    dump_stack ();

protected:
    std::string m_dump_str;

    std::string m_str;
};

__NAMESPACE_END

#endif /* _EXCEPTION_H_ */
