#ifndef _WIN32
//#include <signal.h>
#endif
#include <memory>
#include "core/base/common.h"
#include "core/event/io_thread.h"

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
    if (m_loop and m_loop->looping())
        ignore_excp(m_loop->stop());
}

void
io_thread::run (crash_handler &&_crash_handler /* = nullptr */)
{
    {
        local_lock _lock(m_mutex);
        m_loop = std::make_shared<event_loop>();
        thread::run([this] () -> int {
            return process();
        }, std::move(_crash_handler));
        while (!m_loop->looping())
            m_cond.wait();
    }
}

void
io_thread::stop ()
{
    if (!equal_to(thread::ptid()))
    {
        local_lock _lock(m_mutex);
        m_loop->stop();
        join();
    }
}

int
io_thread::process ()
{
    // shielding SIGPIPE signal
/*
#ifndef _WIN32
    sigset_t _signal_mask;
    sigemptyset(&_signal_mask);
    sigaddset(&_signal_mask, SIGPIPE);
    pthread_sigmask(SIG_BLOCK, &_signal_mask, NULL);
#endif
*/

    m_loop->run([this] () {
        {
            local_lock _lock(m_mutex);
            m_cond.signal();
        }
    }, nullptr);

    return 0;
}

KNGIN_NAMESPACE_K_END
