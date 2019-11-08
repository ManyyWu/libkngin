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

io_threadpool::io_threadpool ()
    try
    : m_size(1), m_max(THREADS_MAX), m_min(1), m_alive(timestamp::infinite()),
      m_conn_size(SIZE_MAX), m_queue_size(SIZE_MAX), m_threads(m_size),
      m_stopped(true)
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
io_threadpool::start ()
{
    try {
        for (int i = 0; i < m_size; ++i) {
            std::string _name = std::string("io_thread_") + std::to_string(i);
            m_threads.push_back(std::make_unique<io_thread>(_name.c_str()));
            if (!m_threads.back().get()->run())
                throw exception("io_threadpool::start() error");
        }
    } catch (...) {
        log_fatal("io_threadpool::start() error");
        throw;
    }
    log_info("thread pool started");
}

__NAMESPACE_END
