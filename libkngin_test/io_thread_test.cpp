//#include <cstdio>
//#include <cstdlib>
//#include <ctime>
//#include "work_thread.h"
//#include "work_task.h"
//#include "msg_test.h"
//
#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin_test/threadpoll_test.cpp"
//
//using namespace std;

//static int
//process (void *args)
//{
//    work_thread *c = (work_thread *)args;
//    srand((unsigned int)time(nullptr));
//    for (int i = 0; i < 100; ++i)
//    {
//        work_task *wt = nullptr;
//        knew(wt, work_task, (c));
//        assert(wt);
//        netmsg_test *msg = nullptr;
//        knew(msg, netmsg_test, (wt));
//        assert(msg->create((ACTION)(rand() % 10), i * rand()));
//        assert(msg);
//        assert(wt->create((msg **)&msg));
//        assert(c->recv_task(&wt));
//        while (!c->task_done());
//        if (c->running()) {
//            // then msg is nullptr
//            msg *reply_msg = c->send_msg();
//            if (!reply_msg)
//                ::fprintf(stderr, "index: %d, no reply, error occured!\n", i);
//            else {
//                ::fprintf(stderr, "index: %d, action: %d, result: %d\n",
//                       i, ((int *)reply_msg->buf())[0], ((int *)reply_msg->buf())[1]);
//                reply_msg->release(); // commonly call by msg filter
//                reply_msg = nullptr;
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
//    t1.join();
//    wt.cancel();
//    wt.join();
//}
