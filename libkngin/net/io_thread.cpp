#include "define.h"
#include "io_thread.h"
#include "common.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin/net/work_thread.cpp"

__NAMESPACE_BEGIN

io_thread::io_thread (const char *_name)
    try
    : thread(_name), m_loop(new event_loop(this)),
      m_mutex(), m_cond(&m_mutex)
{
} catch (...) {
    delete m_loop;
    throw;
}

io_thread::~io_thread ()
{
    if (m_loop && m_loop->looping())
        m_loop->stop();
    safe_release(m_loop);
    if (running()) {
        cancel();
        join(nullptr);
    }
}

bool
io_thread::run ()
{
    local_lock _lock(m_mutex);
    bool _ret = thread::run(std::bind(&io_thread::process, this));
    while (_ret && !m_loop->looping())
        m_cond.wait();

    return _ret;
}

int
io_thread::process ()
{
    m_loop->loop([this] () {
        local_lock _lock(m_mutex);
        m_cond.signal();
    }, nullptr);

    return 0;
}

__NAMESPACE_END
