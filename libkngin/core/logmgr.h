#ifndef _LOGMGR_H_
#define _LOGMGR_H_

#include <vector>
#include <string>
#include <atomic>
#include "log.h"
#include "define.h"
#include "noncopyable.h"

KNGIN_NAMESPACE_K_BEGIN

class log_mgr : public noncopyable {
public:
    typedef std::vector<log *>       kngin_log_set;

    typedef std::vector<std::string> kngin_logfile_set;

private:
    log_mgr     ();

    ~log_mgr    ();

public:
    log &
    operator [] (size_t _index);

    int
    add         (const std::string &_filename, KNGIN_LOG_MODE _mode);

    std::string &
    filename_at (size_t _index);

public:
    bool
    inited      () const;

protected:
    static kngin_log_set     m_log_set;

    static kngin_logfile_set m_logfile_set;

    static std::atomic<bool> m_inited;

private:
    friend log_mgr &
    logger ();
};

log_mgr &
logger ();

KNGIN_NAMESPACE_K_END

#endif /* _LOGMGR_H_ */