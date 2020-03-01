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
thread                     log_mgr::thr_("async logger thread");

log_mgr::async_log_dataq   log_mgr::log_dataq_;

mutex                      log_mgr::mutex_;

cond                       log_mgr::cond_(&log_mgr::mutex_);

std::atomic_bool           log_mgr::stop_(false);
#endif

log_mgr::kngin_log_set     log_mgr::log_set_;

log_mgr::kngin_logfile_set log_mgr::logfile_set_ = {
    "kngin_server", // default
};

std::atomic_bool           log_mgr::inited_(false);

log_mgr::log_callback      log_mgr::log_cb_(nullptr);

log_mgr::log_mgr ()
{
    if (log_mgr::inited_)
        return;

    try {
        // reserved type
        log_set_.push_back(
            std::make_unique<log>(KNGIN_LOG_FILE_SERVER, KNGIN_LOG_MODE_BOTH));
    } catch (std::exception &e) {
        std::cerr << "logger init failed: " << e.what() << std::endl;
        throw;
    } catch (...) {
        std::cerr << "logger init failed" << std::endl;
        throw;
    }
    inited_ = true;

#if (ON == KNGIN_ASYNC_LOGGER)
    thr_.run(&log_mgr::log_thread);
#endif
}

log_mgr::~log_mgr ()
{
#if (ON == KNGIN_ASYNC_LOGGER)
    log_mgr::stop_ = true;
    {
        local_lock lock(log_mgr::mutex_);
    }
    log_mgr::cond_.signal();
    if (!log_mgr::thr_.joined())
        log_mgr::thr_.join();
#else
    inited_ = false;
#endif
}

#if (ON == KNGIN_ASYNC_LOGGER)

void
log_mgr::async_log (log::async_log_data &&data)
{
    if (!log_mgr::inited_)
        return;
    {
        local_lock lock(log_mgr::mutex_);
        log_mgr::log_dataq_.push_front(std::move(data));
    }
    log_mgr::cond_.signal();
}

int
log_mgr::log_thread ()
{
    log::async_log_data fn = nullptr;

    try {
        while (true) {
            {
                // wait
                local_lock lock(log_mgr::mutex_);
                while (log_mgr::log_dataq_.empty() and !log_mgr::stop_) {
                    log_mgr::cond_.timedwait(KNGIN_ASYNC_LOGGER_TIMEOUT);
                }
                if (log_mgr::stop_ and log_mgr::log_dataq_.empty())
                    break;

                // get next
                fn = log_mgr::log_dataq_.back();
                log_mgr::log_dataq_.pop_back();
            }
            // write
            if (fn)
                fn();
            fn = nullptr;
        }
    }
    catch (std::exception &e) {
        std::cerr << "catch an exception in log_thread: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "catch an exception in log_thread" << std::endl;
    }
    inited_ = false;

    return 0;
}

#endif

log &
log_mgr::operator [] (size_t index) noexcept
{
    assert(log_mgr::inited_);
    assert(log_mgr::log_set_.size() == log_mgr::logfile_set_.size());
    return *log_mgr::log_set_[index];
}

const std::string &
log_mgr::filename_at (size_t index) noexcept
{
    assert(log_mgr::inited_);
    assert(index < log_mgr::log_set_.size());
    return log_mgr::logfile_set_[index];
}

log_mgr &
logger ()
{
    static log_mgr logger;

    return logger;
}

KNGIN_NAMESPACE_K_END