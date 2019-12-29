#ifndef KNGIN_WORKER_THERAD_POLL_H
#define KNGIN_WORKER_THERAD_POLL_H

#include <vector>
#include <atomic>
#include <memory>
#include <algorithm>
#include <functional>
#include "core/define.h"
#include "core/lock.h"
#include "core/noncopyable.h"
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

class io_threadpool : public noncopyable {
public:
    typedef event_loop::task                        task;

    typedef std::deque<task>                        task_queue;

    typedef std::vector<std::unique_ptr<io_thread>> threads;

    typedef std::function<void (void)>              crash_handler;

    typedef io_thread::event_loop_ptr               event_loop_ptr;

public:
    io_threadpool  () = delete;

    explicit
    io_threadpool  (uint16_t _num);

    ~io_threadpool ();

public:
    void
    start          (crash_handler _handler);

    void
    stop           ();

    void
    add_task       (task &&_task);

    bool
    stopped        ()
    { return m_stopped; }

    event_loop &
    next_loop      ();

    event_loop &
    get_loop       (size_t _idx);

private:
    const uint16_t    m_num;

    threads           m_threads;

    std::atomic_bool  m_stopped;

    std::atomic_bool  m_crash;

    mutex             m_mutex;

    uint16_t          m_next;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_WORKER_THERAD_POLL_H */