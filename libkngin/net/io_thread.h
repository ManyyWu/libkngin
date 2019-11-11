#ifndef _io_thread_H_
#define _io_thread_H_

#include <memory>
#include "define.h"
#include "thread.h"
#include "lock.h"
#include "event_loop.h"

__NAMESPACE_BEGIN

class io_thread : public thread {
public:
    typedef std::shared_ptr<event_loop> event_loop_ptr;

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

    event_loop_ptr
    get_loop   () { return m_loop; }

protected:
    int
    process    ();

protected:
    event_loop_ptr m_loop;

    mutex          m_mutex;

    cond           m_cond;
};

__NAMESPACE_END

#endif /* _io_thread_H_ */
