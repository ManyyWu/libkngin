#include "define.h"
#include "work_thread.h"
#include "thread.h"
#include "msg.h"

__NAMESPACE_BEGIN

work_thread::work_thread (task *_task)
{
    assert(_task && _task->_pfn && _args);
    thread(this->process, _task)
}

work_thread::~work_thread ()
{
}

msg *
work_thread::send_msg()
{
    m_msg = 
}

__NAMESPACE_END
