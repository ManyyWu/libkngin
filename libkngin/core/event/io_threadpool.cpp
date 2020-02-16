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

io_threadpool::io_threadpool (uint16_t _num)
    try
    : m_num((std::max<uint16_t>)(_num, 1)),
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
io_threadpool::start (crash_handler _handler)
{
    assert(m_stopped);

    for (int i = 0; i < m_num; ++i) {
        auto _name = std::string("io_thread_") + std::to_string(i);
        m_threads.push_back(std::make_unique<io_thread>(_name.c_str()));
        try {
            //log_debug("%s", m_threads.back()->name());
            m_threads.back()->run([this, _handler] (pthread_t _t) {
                if (m_crash)
                    return;
                m_crash = true;
                ignore_excp(_handler());
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
        local_lock _lock(m_mutex);
        size_t _size = m_threads.size();
        for (uint16_t i = 0; i < _size; ++i)
            m_threads[i]->stop();
        m_threads.clear();
    }
    m_stopped = true;
    log_info("thread pool stopped");
}

void
io_threadpool::add_task (task &&_task)
{
    assert(_task);
    assert(!m_stopped);

    event_loop &_next = next_loop();
    {
        local_lock _lock(m_mutex);
        if (_next.looping())
            _next.run_in_loop(std::move(_task));
    }
}

event_loop &
io_threadpool::next_loop ()
{
    assert(!m_stopped);
    {
        local_lock _lock(m_mutex);
        size_t _size = m_threads.size();
        assert(_size);
        if (m_next >= _size)
            m_next = std::max<size_t>(_size - 1, 0);
        return *(m_threads[m_next++]->get_loop());
    }
}

event_loop &
io_threadpool::get_loop (size_t _idx)
{
    assert(!m_stopped);
    {
        local_lock _lock(m_mutex);
        size_t _size = m_threads.size();
        assert(_idx < _size);
        return *(m_threads[_idx]->get_loop());
    }
}

KNGIN_NAMESPACE_K_END
