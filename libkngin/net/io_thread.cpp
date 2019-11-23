#include <memory>
#include "io_thread.h"
#include "common.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin/net/work_thread.cpp"

KNGIN_NAMESPACE_K_BEGIN

io_thread::io_thread (const char *_name)
    try
    : thread(_name),
      m_loop(std::make_shared<event_loop>(this)),
      m_mutex(),
      m_cond(&m_mutex)
{
} catch (...) {
    log_fatal("io_thread::iothread() error");
    throw;
}

io_thread::~io_thread ()
{
    if (m_loop && m_loop->looping())
        m_loop->stop();
    if (!m_joined) {
        log_warning("unjoined thread \"%s\"", name());
        join(nullptr);
    }
}

bool
io_thread::run ()
{
    {
        local_lock _lock(m_mutex);
        if (!thread::run(std::bind(&io_thread::process, this)))
            return false;
        while (!m_loop->looping())
            m_cond.wait();
    }
    return true;
}

void
io_thread::stop ()
{
    {
        local_lock _lock(m_mutex);
        m_loop->stop();
        join(nullptr);
    }
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

KNGIN_NAMESPACE_K_END
