//#ifndef _WORK_THREAD_
//#define _WORK_THREAD_
//
//#include "define.h"
//#include "thread.h"
//#include "msg.h"
//#include "work_task.h"
//#include "mutex.h"
//#include "cond.h"
//
//KNGIN_NAMESPACE_K_BEGIN
//
//class work_thread : public thread {
//public:
//    work_thread (const char *_name);
//
//    virtual
//    ~work_thread ();
//
//public:
//    virtual bool
//    run          ();
//
//    virtual bool
//    cancel       ();
//
//public:
//    bool
//    task_done    () const;
//
//    bool
//    recv_task    (work_task **_task); // call by thread pool main thread
//
//    msg *
//    send_msg     (); // call by thread pool main thread
//
//protected:
//    static void
//    cleanup_lock (void *_args);
//
//public:
//    static int
//    process      (void *_args);
//
//protected:
//    std::atomic<bool>  m_done; // idle and maybe have msg to send
//
//    std::atomic<bool>  m_new_task;
//
//    std::atomic<bool>  m_stop_thread;
//
//    work_task *        m_task;
//
//    mutex              m_mutex;
//
//    cond               m_cond;
//};
//
//KNGIN_NAMESPACE_K_END
//
//#endif /* _WORK_THREAD */
