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
#include "core/base/thread.h"
#include "core/base/mutex.h"
#include "core/base/cond.h"

KNGIN_NAMESPACE_K_BEGIN

class log_mgr : public noncopyable {
public:
    typedef std::deque<log::async_log_data> async_log_dataq;

    typedef std::unique_ptr<log>            log_ptr;

    typedef std::vector<log_ptr>            kngin_log_set;

    typedef std::vector<std::string>        kngin_logfile_set;

private:
    log_mgr     ();

    ~log_mgr    () KNGIN_NOEXCP;

#if (ON == KNGIN_ASYNC_LOGGER)
public:
    void
    async_log   (log::async_log_data &&_data);

private:
    static int
    log_thread  ();
#endif

public:
    log &
    operator [] (size_t _index) KNGIN_NOEXCP;

    int
    add         (const std::string &_filename, KNGIN_LOG_MODE _mode) = delete;

    const std::string &
    filename_at (size_t _index) KNGIN_NOEXCP;

public:
    bool
    inited      () const KNGIN_NOEXCP;

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

private:
    friend log_mgr &
    logger ();
};

log_mgr &
logger ();

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_LOGMGR_H */