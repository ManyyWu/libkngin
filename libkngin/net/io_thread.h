#ifndef _io_thread_H_
#define _io_thread_H_

#include "define.h"
#include "thread.h"
#include "mutex.h"
#include "cond.h"
#include "event_loop.h"

__NAMESPACE_BEGIN

class io_thread : public thread {
    typedef event_loop::loop_started_cb loop_started_cb;

    typedef event_loop::loop_stopped_cb loop_stopped_cb;

public:
    io_thread  (const char *_name,
                loop_started_cb &&_started_cb,
                loop_stopped_cb &&_stopped_cb);

    virtual
    ~io_thread ();

public:
    bool
    run        ()            { return thread::run(std::bind(&io_thread::process, this, nullptr)); }

    event_loop *
    get_loop   ()            { return m_loop; }

protected:
    int
    process    (void *_args) { m_loop->loop(m_started_cb, m_stopped_cb); return 0; }

protected:
    event_loop *    m_loop;

    loop_started_cb m_started_cb;

    loop_stopped_cb m_stopped_cb;
};

__NAMESPACE_END

#endif /* _io_thread_H_ */
