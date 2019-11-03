//#include "define.h"
//#include "task_base.h"
//#include "msg.h"
//#include "work_thread.h"
//
//__NAMESPACE_BEGIN
//
//task_base::task_base (work_thread *_thread, msg *_msg, int _priority /* = 0 */)
//    : m_thread(_thread), m_msg(_msg), m_priority(_priority)
//{
//    check(_thread);
//    // log, throw
//}
//
//task_base::~task_base ()
//{
//    if (m_msg)
//        m_msg->release();
//    m_thread = nullptr;
//}
//
//bool
//task_base::create (msg **_msg)
//{
//    check_r0(_msg);
//    check_r0(*_msg);
//
//    if (m_msg)
//        m_msg->release();
//    m_msg = *_msg;
//    *_msg = nullptr;
//    return true;
//}
//
//void
//task_base::release()
//{
//    kdelete_this(this);
//}
//
//int
//task_base::priority () const
//{
//    return m_priority;
//}
//
//void
//task_base::set_priority (int _priority)
//{
//    m_priority = _priority;
//}
//
//__NAMESPACE_END
