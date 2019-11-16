#ifndef _WORKER_THERAD_POLL_H_
#define _WORKER_THERAD_POLL_H_

#include <vector>
#include <atomic>
#include <memory>
#include <algorithm>
#include <functional>
#include "define.h"
#include "lock.h"
#include "event_loop.h"
#include "io_thread.h"
#include "tcp_connection.h"

#define THREADS_MAX UINT16_MAX

__NAMESPACE_BEGIN

/*
* I/O thread pool
* m_core_size: The number of threads whose will not be destroyed when timeout occurs
* m_max_size:  The number of threads in thread pool
* m_alive:     The maxinum time a thread can live while idle
* */

class io_threadpool {
public:
    typedef event_loop::queued_fn                   task;

    typedef std::deque<task>                        task_queue;

    typedef std::vector<std::unique_ptr<io_thread>> threads;

    typedef event_loop::loop_started_cb             loop_started_cb;

    typedef event_loop::loop_stopped_cb             loop_stopped_cb;

    typedef io_thread::event_loop_ptr               event_loop_ptr;

public:
    io_threadpool  () = delete;

    explicit
    io_threadpool  (uint16_t _num);

    ~io_threadpool ();

public:
    void
    start          ();

    void
    stop           ();

    void
    add_task       (task &&_task);

    event_loop_ptr
    next_loop      ();

    event_loop_ptr
    get_loop       (size_t _idx);

protected:
    const uint16_t    m_num;

    threads           m_threads;

    task_queue        m_taskq;

    std::atomic<bool> m_stopped;

    mutex             m_mutex;

    cond              m_cond;

    uint16_t          m_next;
};

__NAMESPACE_END

#endif /* _WORKER_THERAD_POLL_H_ */