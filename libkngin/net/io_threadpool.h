#ifndef _WORKER_THERAD_POLL_H_
#define _WORKER_THERAD_POLL_H_

#include <vector>
#include <atomic>
#include <memory>
#include <algorithm>
#include <functional>
#include "core/define.h"
#include "core/lock.h"
#include "net/event_loop.h"
#include "net/io_thread.h"
#include "net/tcp/session.h"

#define THREADS_MAX UINT16_MAX

KNGIN_NAMESPACE_K_BEGIN

/*
* I/O thread pool
* m_core_size: The number of threads whose will not be destroyed when timeout occurs
* m_max_size:  The number of threads in thread pool
* m_alive:     The maxinum time a thread can live while idle
* */

class io_threadpool {
public:
    typedef event_loop::task                        task;

    typedef std::deque<task>                        task_queue;

    typedef std::vector<std::unique_ptr<io_thread>> threads;

    typedef event_loop::started_handler             started_handler;

    typedef event_loop::stopped_handler             stopped_handler;

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

KNGIN_NAMESPACE_K_END

#endif /* _WORKER_THERAD_POLL_H_ */