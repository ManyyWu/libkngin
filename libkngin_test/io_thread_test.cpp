//#include <cstdio>
//#include <cstdlib>
//#include <ctime>
//#include "work_thread.h"
//#include "work_task.h"
//#include "msg_test.h"
//
#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin_test/threadpoll_test.cpp"
//
//using namespace std;

//static int
//process (void *_args)
//{
//    work_thread *_c = (work_thread *)_args;
//    srand((unsigned int)time(nullptr));
//    for (int i = 0; i < 100; ++i)
//    {
//        work_task *_wt = nullptr;
//        knew(_wt, work_task, (_c));
//        check(_wt);
//        netmsg_test *_msg = nullptr;
//        knew(_msg, netmsg_test, (_wt));
//        check(_msg->create((ACTION)(rand() % 10), i * rand()));
//        check(_msg);
//        check(_wt->create((msg **)&_msg));
//        check(_c->recv_task(&_wt));
//        while (!_c->task_done());
//        if (_c->running()) {
//            // then _msg is nullptr
//            msg *_reply_msg = _c->send_msg();
//            if (!_reply_msg)
//                ::fprintf(stderr, "index: %d, no reply, error occured!\n", i);
//            else {
//                ::fprintf(stderr, "index: %d, action: %d, result: %d\n",
//                       i, ((int *)_reply_msg->buf())[0], ((int *)_reply_msg->buf())[1]);
//                _reply_msg->release(); // commonly call by msg filter
//                _reply_msg = nullptr;
//            }
//        }
//    }
//
//    return 0;
//}
//
//void
//work_thread_test ()
//{
//    work_thread wt("work_thread_test");
//    thread t1(process, &wt);
//    wt.run();
//    while (!wt.running());
//    t1.run();
//
//    t1.join(nullptr);
//    wt.cancel();
//    wt.join(nullptr);
//}
