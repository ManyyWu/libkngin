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
    task_base (work_thread *_thread)
        : m_thread(_thread)
    {
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
    virtual bool
    create (msg *_msg)
    {
        assert(_msg);

        m_msg->release();
        m_msg = NULL;
        if (!_msg)
            return false;
        m_msg = _msg;
    }

public:
    virtual bool
    process    () = 0;

    virtual bool
    send_msg   (msg *_msg) = 0;

protected:
    msg         *m_msg;

    work_thread *m_thread;
};

__NAMESPACE_END

#endif /* _TASK_BASE_H_ */