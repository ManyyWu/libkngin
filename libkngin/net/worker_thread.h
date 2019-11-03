#ifndef _WORKER_THREAD_H_
#define _WORKER_THREAD_H_

#include "define.h"
#include "thread.h"
#include "mutex.h"
#include "cond.h"
#include "event_loop.h"

__NAMESPACE_BEGIN

class worker_thread : public thread {
    typedef std::function<void (worker_thread *)> thread_inited_cb;

    typedef std::function<void (worker_thread *)> thread_stopped_cb;

public:
    worker_thread       (const char *_name,
                         thread_inited_cb &&_inited_cb,
                         thread_stopped_cb &&_stopped_cb);

    ~worker_thread      ();

protected:
    int
    process             (void *_args);

    void
    handle_loop_started (event_loop *_loop);

    void
    handle_loop_stopped ();

protected:
    event_loop *      m_loop;

    thread_inited_cb  m_inited_cb;

    thread_stopped_cb m_stopped_cb;
};

__NAMESPACE_END

#endif /* _WORKER_THREAD_H_ */
