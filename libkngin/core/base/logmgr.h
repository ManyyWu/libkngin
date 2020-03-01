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
    { return log_mgr::m_inited; }

    // no thread-safe
    void
    set_log_callback (log_callback cb) noexcept
    { m_log_cb = cb; }

    // no thread-safe
    log_callback
    get_log_callback () noexcept
    { return m_log_cb; }

private:
#if (ON == KNGIN_ASYNC_LOGGER)
    static thread            m_thr;

    static async_log_dataq   m_log_dataq;

    static mutex             m_mutex;

    static cond              m_cond;

    static std::atomic_bool  m_stop;
#endif

    static kngin_log_set     m_log_set;

    static kngin_logfile_set m_logfile_set;

    static std::atomic_bool  m_inited;

    static log_callback      m_log_cb;

private:
    friend log_mgr &
    logger ();
};

log_mgr &
logger ();

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_LOGMGR_H */