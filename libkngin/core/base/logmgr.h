#ifndef KNGIN_LOGMGR_H
#define KNGIN_LOGMGR_H

#include <vector>
#include <deque>
#include <string>
#include <atomic>
#include <memory>
#include "core/base/log.h"
#include "core/base/define.h"
#include "core/base/noncopyable.h"

KNGIN_NAMESPACE_K_BEGIN

class thread;
class mutex;
class cond;
class log_mgr : public noncopyable {
public:
#if (ON == KNGIN_ASYNC_LOGGER)
    typedef std::deque<log::async_log_data> async_log_dataq;
#endif

    typedef std::unique_ptr<log>            log_ptr;

    typedef std::vector<log_ptr>            kngin_log_set;

    typedef std::vector<std::string>        kngin_logfile_set;

    typedef void (*log_callback) (const char *file, KNGIN_LOG_LEVEL level,
                                  const char *str, size_t len);

private:
    log_mgr          ();

    ~log_mgr         ();

#if (ON == KNGIN_ASYNC_LOGGER)
public:
    void
    async_log        (log::async_log_data &&data);

private:
    static int
    log_thread       ();
#endif

public:
    log &
    operator []      (size_t index) noexcept;

    int
    add              (const std::string &filename, KNGIN_LOG_MODE mode) = delete;

    const std::string &
    filename_at      (size_t index) noexcept;

public:
    bool
    inited           () const noexcept
    { return log_mgr::inited_; }

    // no thread-safe
    void
    set_log_callback (log_callback cb) noexcept
    { log_cb_ = cb; }

    // no thread-safe
    log_callback
    get_log_callback () noexcept
    { return log_cb_; }

private:
#if (ON == KNGIN_ASYNC_LOGGER)
    static thread            thr_;

    static async_log_dataq   log_dataq_;

    static mutex             mutex_;

    static cond              cond_;

    static std::atomic_bool  stop_;
#endif

    static kngin_log_set     log_set_;

    static kngin_logfile_set logfile_set_;

    static std::atomic_bool  inited_;

    static log_callback      log_cb_;

private:
    friend log_mgr &
    logger ();
};

log_mgr &
logger ();

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_LOGMGR_H */