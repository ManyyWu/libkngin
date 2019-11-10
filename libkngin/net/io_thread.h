#ifndef _io_thread_H_
#define _io_thread_H_

#include "define.h"
#include "thread.h"
#include "lock.h"
#include "event_loop.h"

__NAMESPACE_BEGIN

class io_thread : public thread {
public:
    explicit
    io_thread  (const char *_name);

    virtual
    ~io_thread ();

public:
    bool
    run        ();

    void
    stop       ();

    event_loop *
    get_loop   () { return m_loop; }

protected:
    int
    process    ();

protected:
    event_loop * m_loop;

    mutex        m_mutex;

    cond         m_cond;
};

__NAMESPACE_END

#endif /* _io_thread_H_ */
