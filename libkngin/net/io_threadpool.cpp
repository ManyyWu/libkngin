#include <vector>
#include <atomic>
#include <memory>
#include <functional>
#include "define.h"
#include "io_thread.h"
#include "exception.h"
#include "io_threadpool.h"

#define __FILENAME__ "io_threadpool.cpp"

__NAMESPACE_BEGIN

io_threadpool::io_threadpool (uint16_t _min, uint16_t _max)
    try
    : m_min(_min), m_max(_max), m_threads(), m_stopped(true)
{
} catch (...) {
    log_fatal("io_threadpool::io_threadpool() error");
    throw;
}

io_threadpool::~io_threadpool ()
{
    m_threads.clear();
}

void
io_threadpool::start (inited_cb &&_cb)
{
    try {
        for (int i = 0; i < m_num; ++i) {
            std::string _name = std::string("io_thread_") + std::to_string(i);
            m_threads.push_back(std::make_unique<io_thread>(_name.c_str()));
            if (!m_threads.back().get()->run())
                throw exception("io_threadpool::start() error");
        }
        if (_cb && m_threads.size() == m_num)
            _cb(this);
    } catch (...) {
        log_fatal("io_threadpool::start() error");
        throw;
    }
    log_info("thread pool started");
}

__NAMESPACE_END
