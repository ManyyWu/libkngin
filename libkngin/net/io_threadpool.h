#ifndef _WORKER_THERAD_POLL_H_
#define _WORKER_THERAD_POLL_H_

#include <vector>
#include <atomic>
#include <memory>
#include <functional>
#include "define.h"
#include "lock.h"
#include "event_loop.h"
#include "io_thread.h"

__NAMESPACE_BEGIN

class io_threadpool {
public:
    typedef std::vector<std::unique_ptr<io_thread>> threads;

    typedef event_loop::loop_started_cb             loop_started_cb;

    typedef event_loop::loop_stopped_cb             loop_stopped_cb;

public:
    typedef std::function<void (io_threadpool &_pool)> inited_cb;

public:
    io_threadpool  (uint16_t _max, inited_cb &&_cb);

    ~io_threadpool ();

public:
    void
    start           ();

protected:
    int
    assign_loop     ();

    void
    on_loop_start   ();

    void
    on_loop_stop    ();

protected:
    const uint16_t    m_num;

    threads           m_threads;

    std::atomic<bool> m_stopped;

    mutex             m_mutex;

    cond              m_cond;

    inited_cb         m_inited_cb;
};

__NAMESPACE_END

#endif /* _WORKER_THERAD_POLL_H_ */