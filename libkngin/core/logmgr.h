#ifndef _LOGMGR_H_
#define _LOGMGR_H_

#include <vector>
#include <string>
#include <atomic>
#include "log.h"
#include "define.h"
#include "noncopyable.h"

__NAMESPACE_BEGIN

typedef std::vector<log *>       __log_set;
typedef std::vector<std::string> __logfile_set;

class log_mgr : public noncopyable {
private:
    log_mgr     ();

    ~log_mgr    ();

public:
    log *
    operator [] (size_t _index);

    log *
    at          (size_t _index);

    std::string &
    filename_at (size_t _index);

public:
    bool
    inited      () const;

protected:
    static __log_set         m_log_set;

    static __logfile_set     m_logfile_set;

    static std::atomic<bool> m_inited;

private:
    friend log_mgr &
    logger ();
};

log_mgr &
logger ();

__NAMESPACE_END

#endif /* _LOGMGR_H_ */