#include "define.h"
#include "logfile.h"
#include "work_task.h"
#include "thread.h"
#include "msg.h"

__NAMESPACE_BEGIN

work_task::work_task(work_thread *_thread)
    : task_base(_thread, NULL)
{
}

work_task::~work_task()
{
}

bool
work_task::process ()
{
    kassert_r0(m_msg);

    msg *_msg = m_msg;
    m_msg = NULL;
    bool _ret = _msg->process();
    if (!_ret)
        _msg->dump();
    _msg->release();
    _msg = NULL;
    return _ret;
}

bool
work_task::create (msg **_msg)
{
    kassert_r0(_msg);
    if (!_msg)
        return false;

    m_msg = *_msg;
    *_msg = NULL;
    return true;
}

void
work_task::release ()
{
    delete this;
}

bool
work_task::recv_reply_msg (msg **_msg)
{
    kassert_r0(_msg);
    kassert_r0(!m_msg);

    m_msg = *_msg;
    *_msg = NULL;
}

msg *
work_task::send_reply_msg ()
{
    // kassert_r0(m_msg);
    msg *_msg = m_msg;
    m_msg = NULL;
    return _msg;
}

__NAMESPACE_END
