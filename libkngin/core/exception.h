#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <exception>
#include <stdexcept>
#include "define.h"

#define __EXP   noexcept(false)
#define __NOEXP noexcept(true)

__NAMESPACE_BEGIN

class logic_exception : public std::logic_error {
public:
    logic_exception  (std::string _what, const char *_file, int _line);

    virtual
    ~logic_exception ();

public:
    virtual const char *
    what             () const __NOEXP;

    const char *
    dump             () const;

protected:
    void
    dump_stack       ();

protected:
    std::string m_dump_str;
};

__NAMESPACE_END

#endif /* _EXCEPTION_H_ */
