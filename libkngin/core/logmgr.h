#ifndef KNGIN_LOGMGR_H
#define KNGIN_LOGMGR_H

#include <vector>
#include <string>
#include <atomic>
#include <memory>
#include "core/log.h"
#include "core/define.h"
#include "core/noncopyable.h"

KNGIN_NAMESPACE_K_BEGIN

class log_mgr : public noncopyable {
public:
    typedef std::unique_ptr<log>     log_ptr;

    typedef std::vector<log_ptr>     kngin_log_set;

    typedef std::vector<std::string> kngin_logfile_set;

private:
    log_mgr     ();

    ~log_mgr    () KNGIN_NOEXCP;

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