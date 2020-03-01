#ifndef KNGIN_WORKER_THERAD_POLL_H
#define KNGIN_WORKER_THERAD_POLL_H

#include <vector>
#include <atomic>
#include <memory>
#include <algorithm>
#include <functional>
#include "core/base/define.h"
#include "core/base/lock.h"
#include "core/base/noncopyable.h"
#include "core/event/event_loop.h"
#include "core/event/io_thread.h"

#define THREADS_MAX UINT16_MAX

KNGIN_NAMESPACE_K_BEGIN

/*
 * I/O thread pool
 * core_size_: The number of threads whose will not be destroyed when timeout occurs
 * max_size_:  The number of threads in thread pool
 * alive_:     The maxinum time a thread can live while idle
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
    io_threadpool  (uint16_t num);

    ~io_threadpool ();

public:
    void
    start          (crash_handler handler);

    void
    stop           ();

    void
    add_task       (task &&task);

    bool
    stopped        ()
    { return stopped_; }

    event_loop &
    next_loop      ();

    event_loop &
    get_loop       (size_t idx);

private:
    const uint16_t    num_;

    threads           threads_;

    std::atomic_bool  stopped_;

    std::atomic_bool  crash_;

    mutex             mutex_;

    size_t            next_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_WORKER_THERAD_POLL_H */