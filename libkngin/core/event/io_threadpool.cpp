#include <vector>
#include <atomic>
#include <memory>
#include <algorithm>
#include "core/base/exception.h"
#include "core/base/common.h"
#include "core/event/io_thread.h"
#include "core/event/io_threadpool.h"

#define KNGIN_FILENAME "libkngin/core/event/io_threadpool.cpp"

KNGIN_NAMESPACE_K_BEGIN

io_threadpool::io_threadpool (uint16_t num)
    try
    : m_num((std::max<uint16_t>)(num, 1)),
      m_threads(),
      m_stopped(true),
      m_crash(false),
      m_mutex(),
      m_next(1)
{
    m_threads.reserve(m_num);
} catch (...) {
    log_fatal("io_threadpool::io_threadpool() error");
    throw;
}

io_threadpool::~io_threadpool ()
{
    if (!m_stopped)
        ignore_excp(stop());
}

void
io_threadpool::start (crash_handler handler)
{
    assert(m_stopped);

    for (int i = 0; i < m_num; ++i) {
        auto name = std::string("io_thread_") + std::to_string(i);
        m_threads.push_back(std::make_unique<io_thread>(name.c_str()));
        try {
            //log_debug("%s", m_threads.back()->name());
            m_threads.back()->run([this, handler] (pthread_t t) {
                if (m_crash)
                    return;
                m_crash = true;
                ignore_excp(handler());
                m_stopped = false;
            }); // end of crash_handler
        } catch (...) {
            m_stopped = false;
            ignore_excp(stop());
            throw k::exception("thread pool startup failed");
        }
    }

    m_stopped = false;
    log_info("thread pool started");
}

void
io_threadpool::stop ()
{
    assert(!m_stopped);

    {
        local_lock lock(m_mutex);
        auto size = m_threads.size();
        for (uint16_t i = 0; i < size; ++i)
            m_threads[i]->stop();
        m_threads.clear();
    }
    m_stopped = true;
    log_info("thread pool stopped");
}

void
io_threadpool::add_task (task &&task)
{
    assert(task);
    assert(!m_stopped);

    event_loop &next = next_loop();
    {
        local_lock lock(m_mutex);
        if (next.looping())
            next.run_in_loop(std::move(task));
    }
}

event_loop &
io_threadpool::next_loop ()
{
    assert(!m_stopped);
    {
        local_lock lock(m_mutex);
        size_t size = m_threads.size();
        assert(size);
        if (m_next >= size)
            m_next = std::max<size_t>(size - 1, 0);
        return *(m_threads[m_next++]->get_loop());
    }
}

event_loop &
io_threadpool::get_loop (size_t idx)
{
    assert(!m_stopped);
    {
        local_lock lock(m_mutex);
        size_t size = m_threads.size();
        assert(idx < size);
        return *(m_threads[idx]->get_loop());
    }
}

KNGIN_NAMESPACE_K_END
