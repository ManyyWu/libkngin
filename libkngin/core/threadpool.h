#ifndef _THREADPOLL_H_
#define _THREADPOLL_H_

#include <vector>
#include <deque>
#include <atomic>
#include "thread.h"
#include "task_base.h"
#include "lock.h"
#include "sync_queue.h"

__NAMESPACE_BEGIN

class thread_pool {
//join_all 无序 超时
//cancel_all 无序 超时

protected:
    std::atomic<bool>       m_running;

    std::atomic<int>        m_maxthread;

    std::vector<thread *>   m_pool;

    sync_deque<task_base *> m_recv_queue;

    sync_deque<task_base *> m_send_queue;
};

__NAMESPACE_END

#endif /* _THREADPOLL_H_ */
