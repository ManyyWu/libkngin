#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <string>
#include "define.h"

#define KNGIN_EXP   noexcept(false)
#define KNGIN_NOEXP noexcept(true)

KNGIN_NAMESPACE_K_BEGIN

class exception {
public:
    exception  () = delete;

    explicit
    exception  (const char *_what);

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

KNGIN_NAMESPACE_K_END

#endif /* _EXCEPTION_H_ */
