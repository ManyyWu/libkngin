#include <vector>
#include <atomic>
#include <memory>
#include <functional>
#include <algorithm>
#include "io_thread.h"
#include "exception.h"
#include "io_threadpool.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "io_threadpool.cpp"

__NAMESPACE_BEGIN

io_threadpool::io_threadpool (uint16_t _num)
    try
    : m_num((std::max<uint16_t>)(_num, 1)),
      m_threads(),
      m_taskq(),
      m_stopped(true),
      m_mutex(),
      m_cond(&m_mutex),
      m_next(1)
{
} catch (...) {
    log_fatal("io_threadpool::io_threadpool() error");
    throw;
}

io_threadpool::~io_threadpool ()
{
    if (!m_stopped)
        stop();
}

void
io_threadpool::start ()
{
    check(m_stopped);

    try {
        for (int i = 0; i < m_num; ++i) {
            std::string _name = std::string("io_thread_") + std::to_string(i);
            m_threads.push_back(std::make_unique<io_thread>(_name.c_str()));
            if (!m_threads.back().get()->run())
                throw exception("io_threadpool::start() error");
        }
    } catch (...) {
        log_fatal("io_threadpool::start() error");
        throw;
    }
    m_stopped = false;
    log_info("thread pool started");
}

void
io_threadpool::stop ()
{
    check(!m_stopped);

    {
        local_lock _lock(m_mutex);
        for (uint16_t i = 0; i < m_threads.size(); ++i)
            m_threads[i]->stop();
        m_threads.clear();
        m_taskq.clear();
    }
    m_stopped = true;
    log_info("thread pool stopped");
}

void
io_threadpool::add_task (task &&_task)
{
    check(_task);

    event_loop_ptr _next = next_loop();
    {
        local_lock _lock(m_mutex);
        if (_next && _next->looping())
            _next->run_in_loop(std::move(_task));
    }
}

io_threadpool::event_loop_ptr
io_threadpool::next_loop ()
{
    {
        local_lock _lock(m_mutex);
        if (m_threads.empty())
            return nullptr;
        size_t _size = m_threads.size();
        if (m_next >= _size)
            m_next = 0;
        //    m_next = (std::min<size_t>)(_size - 1, 1);
        return m_threads[m_next++]->get_loop();
    }
}

io_threadpool::event_loop_ptr
io_threadpool::get_loop (size_t _idx)
{
    {
        local_lock _lock(m_mutex);
        if (m_threads.empty())
            return nullptr;
        size_t _size = m_threads.size();
        check(_idx < _size);
        return m_threads[_idx]->get_loop();
    }
}
__NAMESPACE_END
