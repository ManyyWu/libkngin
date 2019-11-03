#include "define.h"
#include "worker_thread.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin/net/work_thread.cpp"

__NAMESPACE_BEGIN

worker_thread::worker_thread (const char *_name,
                              thread_inited_cb &&_inited_cb,
                              thread_stopped_cb &&_stopped_cb)
    : thread(_name), m_inited_cb(_inited_cb), m_stopped_cb(_stopped_cb)
{
}

worker_thread::~worker_thread ()
{
}

int
worker_thread::process (void *_args)
{
    event_loop _loop(this);
    _loop.loop(std::bind(&worker_thread::handle_loop_started, this, std::placeholders::_1),
               std::bind(&worker_thread::handle_loop_stopped, this));
    m_loop = NULL;
}

void
worker_thread::handle_loop_started (event_loop *_loop)
{
    m_loop = _loop;
    if (m_inited_cb)
        m_inited_cb(this);
}

void
worker_thread::handle_loop_stopped ()
{
    m_loop = NULL;
    if (m_stopped_cb)
        m_stopped_cb(this);
}

__NAMESPACE_END
