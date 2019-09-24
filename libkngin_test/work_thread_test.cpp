#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "../libkngin/core/work_thread.h"
#include "../libkngin/core/work_task.h"
#include "msg_test.h"

static int
process (void *_args)
{
    work_thread *_c = (work_thread *)_args;
    srand(time(NULL));
    for (int i = 0; i < 100; i++)
    {
        work_task *_wt = new work_task(_c);
        kassert(_wt);
        netmsg_test *_msg = new netmsg_test(_wt);
        kassert(_msg->create((ACTION)(rand() % 4), i * rand()));
        kassert(_msg);
        kassert(_wt->create((msg **)&_msg));
        kassert(_c->recv_task(&_wt));
        while (!_c->task_done());
        if (_c->running()) {
            // then _msg is NULL
            msg *_reply_msg = _c->send_msg();
            if (!_reply_msg)
                printf("index: %d, no reply, error occured!\n", i);
            else {
                printf("index: %d, action: %d, result: %d\n",
                       i, ((int *)_reply_msg->buf())[0], ((int *)_reply_msg->buf())[1]);
                _reply_msg->release(); // commonly call by msg filter
            }
        }
    }

    return 0;
}

void
work_thread_test ()
{
    work_thread wt;
    thread t1(process, &wt);
    wt.run();
    while (!wt.running());
    t1.run();

    t1.join(NULL);
    wt.cancel();
    wt.join(NULL);
}
