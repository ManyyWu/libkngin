#ifndef _WORK_TASK_H_
#define _WORK_TASK_H_

#include "define.h"
#include "task_base.h"
#include "thread.h"
#include "msg.h"

__NAMESPACE_BEGIN

class work_task : public task_base {
public:
    work_task      (work_thread *_thread);

protected:
    virtual
    ~work_task     ();

public:
    virtual bool
    process        ();

    virtual bool
    create         (msg **_msg);

    virtual void
    release        ();

    virtual bool
    recv_reply_msg (msg **_msg);

    virtual msg *
    send_reply_msg ();
};

__NAMESPACE_END

#endif /* _WORK_TASK_H_ */