#ifndef _TASK_BASE_H_
#define _TASK_BASE_H_

#include <atomic>
#include "define.h"
#include "noncopyable.h"
#include "msg.h"

__NAMESPACE_BEGIN

class work_thread;
class task_base : noncopyable {
public:
    task_base (msg *_msg, work_thread *_thread)
        : m_msg (_msg), m_thread(_thread)
    {
        assert(_msg);
        assert(_thread);
    }

protected:
    virtual
    ~task_base ()
    {
        if (m_msg)
            m_msg->release();
        m_thread = NULL;
    }

public:
    virtual int
    process    () = 0;

    virtual bool
    send_msg   () = 0;

protected:
    msg         *m_msg;

    task_base   *m_task;

    work_thread *m_thread;
};

__NAMESPACE_END

#endif /* _TASK_BASE_H_ */