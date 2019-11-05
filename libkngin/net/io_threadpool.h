#ifndef _WORKER_THERAD_POLL_H_
#define _WORKER_THERAD_POLL_H_

#include <vector>
#include <atomic>
#include <functional>
#include "define.h"
#include "event_loop.h"
#include "io_thread.h"

__NAMESPACE_BEGIN

class io_thread_pool {
public:
    typedef std::function<void (io_thread_pool &_pool)> inited_cb;

public:
    io_thread_pool  (uint16_t _max, inited_cb &&_cb);

    ~io_thread_pool ();

public:


protected:
    int
    assign_loop         ();

protected:
    std::vector<io_thread> m_threads;

    std::vector<event_loop *>  m_loops;

    std::atomic<bool>          m_stopped;

    inited_cb                  m_inited_cb;
};

__NAMESPACE_END

#endif /* _WORKER_THERAD_POLL_H_ */