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
    exception  (const char *_what);

    ~exception ();

public:
    const std::string &
    what       () const;

    const std::string &
    dump       () const;

protected:
    void
    dump_stack ();

protected:
    std::string m_dump_str;

    std::string m_str;
};

__NAMESPACE_END

#endif /* _EXCEPTION_H_ */
