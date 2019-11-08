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

class io_threadpool {
public:
    typedef std::vector<std::unique_ptr<io_thread>> threads;

    typedef event_loop::loop_started_cb             loop_started_cb;

    typedef event_loop::loop_stopped_cb             loop_stopped_cb;

    typedef event_loop::queued_fn                   task;

    typedef std::function<void (io_threadpool *)>   inited_cb;

public:
    io_threadpool  ();

    ~io_threadpool ();

public:
    void
    start          ();

    void
    stop           ();

    void
    add_task       (task &&_task);

    void
    add_conn       (tcp_connection &_conn);

    void
    remove_conn    (tcp_connection &_conn);

public:
    size_t
    conn_num       ();

    size_t
    task_num       ();

    uint16_t
    size           ();

    uint16_t
    actived        ();

public:
    void
    set_max        (uint16_t _n)      { m_max = (std::min<uint16_t>)(_n, 1); }
    void
    set_core       (uint16_t _n)      { m_min = (std::max<uint16_t>)(_n, 1); }
    void
    set_alive_time (timestamp _alive) { m_alive = _alive; }
    void
    set_queue_max  (size_t _n)        { m_queue_size = (std::max<size_t>)(_n, 1); }
    void
    set_conn_max   (size_t _n)        { m_conn_size = (std::max<size_t>)(_n, 1); }
    uint16_t
    max            () const           { return m_max; }
    uint16_t
    core           () const           { return m_min; }
    timestamp
    alive_time     () const           { return m_alive; }
    size_t
    queue_max      () const           { return m_queue_size; }
    size_t
    conn_max       () const           { return m_conn_size; }

protected:
    size_t            m_size;

    uint16_t          m_max;

    uint16_t          m_min;

    timestamp         m_alive;

    size_t            m_conn_size;

    size_t            m_queue_size;

    threads           m_threads;

    std::atomic<bool> m_stopped;
};

__NAMESPACE_END

#endif /* _WORKER_THERAD_POLL_H_ */