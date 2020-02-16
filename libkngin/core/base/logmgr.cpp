#include <iostream>
#include <string>
#include <cassert>
#include <exception>
#include "core/base/log.h"
#include "core/base/logmgr.h"
#include "core/base/local_lock.h"
#if (ON == KNGIN_ASYNC_LOGGER)
#include "core/base/thread.h"
#include "core/base/mutex.h"
#include "core/base/cond.h"
#endif

KNGIN_NAMESPACE_K_BEGIN

#if (ON == KNGIN_ASYNC_LOGGER)
thread                     log_mgr::m_thr("async logger thread");

log_mgr::async_log_dataq   log_mgr::m_log_dataq;

mutex                      log_mgr::m_mutex;

cond                       log_mgr::m_cond(&log_mgr::m_mutex);

std::atomic_bool           log_mgr::m_stop(false);
#endif

log_mgr::kngin_log_set     log_mgr::m_log_set;

log_mgr::kngin_logfile_set log_mgr::m_logfile_set = {
    "kngin_server", // default
};

std::atomic_bool           log_mgr::m_inited(false);

log_mgr::log_callback      log_mgr::m_log_cb(nullptr);

log_mgr::log_mgr ()
{
    if (log_mgr::m_inited)
        return;

    try {
        // reserved type
        m_log_set.push_back(
            std::make_unique<log>(KNGIN_LOG_FILE_SERVER, KNGIN_LOG_MODE_BOTH));
    } catch (std::exception &_e) {
        std::cerr << "logger init failed: " << _e.what() << std::endl;
        throw;
    } catch (...) {
        std::cerr << "logger init failed" << std::endl;
        throw;
    }
    m_inited = true;

#if (ON == KNGIN_ASYNC_LOGGER)
    m_thr.run(&log_mgr::log_thread);
#endif
}

log_mgr::~log_mgr ()
{
#if (ON == KNGIN_ASYNC_LOGGER)
    log_mgr::m_stop = true;
    {
        local_lock _lock(log_mgr::m_mutex);
    }
    log_mgr::m_cond.signal();
    if (!log_mgr::m_thr.joined())
        log_mgr::m_thr.join();
#else
    m_inited = false;
#endif
}

#if (ON == KNGIN_ASYNC_LOGGER)

void
log_mgr::async_log (log::async_log_data &&_data)
{
    if (!log_mgr::m_inited)
        return;
    {
        local_lock _lock(log_mgr::m_mutex);
        log_mgr::m_log_dataq.push_front(std::move(_data));
    }
    log_mgr::m_cond.signal();
}

int
log_mgr::log_thread ()
{
    log::async_log_data _fn = nullptr;

    try {
        while (true) {
            {
                // wait
                local_lock _lock(log_mgr::m_mutex);
                while (log_mgr::m_log_dataq.empty() and !log_mgr::m_stop) {
                    log_mgr::m_cond.timedwait(KNGIN_ASYNC_LOGGER_TIMEOUT);
                }
                if (log_mgr::m_stop and log_mgr::m_log_dataq.empty())
                    break;

                // get next
                _fn = log_mgr::m_log_dataq.back();
                log_mgr::m_log_dataq.pop_back();
            }
            // write
            if (_fn) 
                _fn();
            _fn = nullptr;
        }
    }
    catch (std::exception &_e) {
        std::cerr << "catch an exception in log_thread: " << _e.what() << std::endl;
    } catch (...) {
        std::cerr << "catch an exception in log_thread" << std::endl;
    }
    m_inited = false;

    return 0;
}

#endif

log &
log_mgr::operator [] (size_t _index) KNGIN_NOEXCP
{
    assert(log_mgr::m_inited);
    assert(log_mgr::m_log_set.size() == log_mgr::m_logfile_set.size());
    return *log_mgr::m_log_set[_index];
}

const std::string &
log_mgr::filename_at (size_t _index) KNGIN_NOEXCP
{
    assert(log_mgr::m_inited);
    assert(_index < log_mgr::m_log_set.size());
    return log_mgr::m_logfile_set[_index];
}

log_mgr &
logger ()
{
    static log_mgr _logger;

    return _logger;
}

KNGIN_NAMESPACE_K_END