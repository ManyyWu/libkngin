#ifndef KNGIN_IO_THREAD_H
#define KNGIN_IO_THREAD_H

#include <memory>
#include "core/define.h"
#include "core/thread.h"
#include "core/lock.h"
#include "net/event_loop.h"

KNGIN_NAMESPACE_K_BEGIN

class io_thread : public thread {
public:
    typedef std::shared_ptr<event_loop> event_loop_ptr;

public:
    explicit
    io_thread  (const char *_name);

    virtual
    ~io_thread ();

public:
    void
    run        (crash_handler &&_handler = nullptr);

    void
    stop       ();

    event_loop_ptr &
    get_loop   ()
    { return m_loop; }

private:
    int
    process    ();

private:
    event_loop_ptr m_loop;

    mutex          m_mutex;

    cond           m_cond;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_IO_THREAD_H */
