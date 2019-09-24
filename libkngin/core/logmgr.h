#ifndef _LOGMGR_H_
#define _LOGMGR_H_

#include <vector>
#include <string>
#include "log.h"
#include "define.h"
#include "noncopyable.h"

__NAMESPACE_BEGIN

class log_mgr : public noncopyable {
public:
    typedef std::vector<log *>       __log_set;
    typedef std::vector<std::string> __logfile_set;

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

protected:
    static __log_set     m_log_set;

    static __logfile_set m_logfile_set;

private:
    friend log_mgr &
    logger ();
};

log_mgr &
logger ();

__NAMESPACE_END

#endif /* _LOGMGR_H_ */