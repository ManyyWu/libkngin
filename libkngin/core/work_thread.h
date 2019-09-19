#ifndef _WORK_THREAD_
#define _WORK_THREAD_

#include "define.h"
#include "thread.h"
#include "msg.h"
#include "task_base.h"

__NAMESPACE_BEGIN

class work_thread : thread {
public:
    work_thread ();

    virtual 
    ~work_thread ();

public:
    virtual msg *
    send_msg (); // return msg to pool, and set m_ret_msg as NULL

    virtual int
    process  (void *); // release task

protected:
    std::atomic<bool>  m_done; // idle and maybe have msg to send

    msg *              m_ret_msg;

    task_base *        m_task;
};

__NAMESPACE_END

#endif /* _WORK_THREAD */
