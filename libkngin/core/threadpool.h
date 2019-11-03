#ifndef _THREADPOLL_H_
#define _THREADPOLL_H_
/*
#include <vector>
#include <deque>
#include <atomic>
#include "define.h"
#include "sync_queue.h"
#include "sync_deque.h"
#include "work_thread.h"
#include "task_base.h"
#include "error.h"
#include "thread.h"
#include "msg.h"
#include "lock.h"
#include "timestamp.h"

__NAMESPACE_BEGIN

#define THREAD_NUM_MAX 65535

class thread_pool : noncopyable {
public:
    typedef sync_deque<task_base> task_queue;

    typedef sync_deque<msg>       msg_queue;

    typedef std::vector<thread *> pool_vector;

public:
    thread_pool  (size_t _qsize, int _thr_max, time_t _alive);

    ~thread_pool ();

public:
    virtual bool
    run           (int _num);

    virtual bool
    stop          ();

    void
    clear         ();

    bool
    commit        (task_base **_task, time_t _ms = TIME_INFINITE);

    msg *
    get_msg       (time_t _ms = TIME_INFINITE);

public:
    bool
    running       () const;

    int
    (max)         () const;

    void
    set_max       (int _max);

    size_t
    queue_max     () const;

    void
    set_queue_max (size_t _max);

public:
    static int
    process       (void *_args);

protected:
    std::atomic<bool>     m_running;

    std::atomic<bool>     m_stop;

    std::atomic<int>      m_maxthread;

    std::atomic<size_t>   m_queue_size;

    std::atomic<size_t>   m_serial;

    time_t                m_alive;

    pool_vector           m_pool;

    task_queue *          m_task_queue;

    msg_queue *           m_msg_queue;

    thread *              m_pool_thread;
};

__NAMESPACE_END
*/

#endif /* _THREADPOLL_H_ */
