#include <vector>
#include <atomic>
#include <memory>
#include <functional>
#include "io_thread.h"
#include "io_threadpool.h"

__NAMESPACE_BEGIN

io_threadpool::io_threadpool (uint16_t _max, inited_cb &&_cb)
    try
    : m_num(_max), m_threads(), m_stopped(true),
      m_mutex(), m_cond(&m_mutex), m_inited_cb(std::move(_cb))
{
} catch (...) {
    log_fatal("io_threadpool::io_threadpool() error");
    throw;
}

io_threadpool::~io_threadpool ()
{
}

void
io_threadpool::start ()
{
    try {
        local_lock _lock(m_mutex);
        for (int i = 0; i < m_num; ++i) {
            std::string _name = std::string("io_thread_") + std::to_string(i);
            m_threads.push_back(std::make_unique<io_thread>( _name.c_str(),
                                    std::bind(&io_threadpool::on_loop_start, this),
                                    std::bind(&io_threadpool::on_loop_stop, this)));
            m_threads.back().get()->run();
        }
    } catch (...) {
        log_fatal("io_threadpool::start()");
        throw;
    }
}

void
io_threadpool::on_loop_start ()
{
    if (m_inited_cb && m_threads.size() == m_num) {
        m_inited_cb(std::ref(*this));
        m_inited_cb = nullptr;
    }
}

void
io_threadpool::on_loop_stop ()
{
}

__NAMESPACE_END
