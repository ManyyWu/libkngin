#ifndef _THREADPOLL_H_
#define _THREADPOLL_H_

#include <vector>
#include <atomic>
#include "thread.h"
#include "task_base.h"
#include "lock.h"

__NAMESPACE_BEGIN

class thread_pool {
//join_all 无序 超时
//cancel_all 无序 超时

protected:
    std::atomic<bool> m_running;


};

__NAMESPACE_END

#endif /* _THREADPOLL_H_ */
