#include <memory>
#include "core/common.h"
#include "net/io_thread.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/work_thread.cpp"

KNGIN_NAMESPACE_K_BEGIN

io_thread::io_thread (const char *_name)
    try
    : thread(_name),
      m_loop(nullptr),
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
        ignore_exp(m_loop->stop());
}

void
io_thread::run ()
{
    {
        local_lock _lock(m_mutex);
        m_loop = std::make_shared<event_loop>(thread::pimpl());
        thread::run(std::bind(&io_thread::process, this));
        while (!m_loop->looping())
            m_cond.wait();
    }
}

void
io_thread::stop ()
{
    {
        local_lock _lock(m_mutex);
        m_loop->stop();
        join();
    }
}

int
io_thread::process ()
{
    m_loop->run([this] () {
        local_lock _lock(m_mutex);
        m_cond.signal();
    }, nullptr);

    return 0;
}

KNGIN_NAMESPACE_K_END
