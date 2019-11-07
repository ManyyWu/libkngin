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
    typedef std::function<void (io_threadpool *)>   inited_cb;

public:
    io_threadpool  (uint16_t _max);

    ~io_threadpool ();

public:
    void
    start           (inited_cb &&_cb);

protected:
    const uint16_t    m_num;

    threads           m_threads;

    std::atomic<bool> m_stopped;

    inited_cb         m_inited_cb;
};

__NAMESPACE_END

#endif /* _WORKER_THERAD_POLL_H_ */