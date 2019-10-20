//#ifndef _TASK_BASE_H_
//#define _TASK_BASE_H_
//
//#include <atomic>
//#include "define.h"
//#include "noncopyable.h"
//#include "msg.h"
//
//__NAMESPACE_BEGIN
//
//class work_thread;
//class task_base : noncopyable {
//public:
//    task_base      () = delete;
//
//    explicit
//    task_base      (work_thread *_thread, msg *_msg, int _priority = 0);
//
//protected:
//    virtual
//    ~task_base     ();
//
//public:
//    virtual bool
//    create         (msg **_msg);
//
//    virtual void
//    release        ();
//
//public:
//    virtual bool
//    process        () = 0;
//
//    virtual bool
//    recv_reply_msg (msg **_msg) = 0;
//
//    virtual msg *
//    send_reply_msg () = 0;
//
//public:
//    int
//    priority       () const;
//
//    void
//    set_priority   (int _priority);
//
//protected:
//    msg *                 m_msg;
//
//    work_thread *         m_thread;
//
//    int                   m_priority;
//};
//
//__NAMESPACE_END
//
//#endif /* _TASK_BASE_H_ */