#ifndef _LOGMGR_H_
#define _LOGMGR_H_

#include <vector>
#include <string>
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
    log &
    operator [] (int _index);

    log &
    at          (int _index);

    std::string &
    filename_at (int _index);

protected:
    static __log_set     m_log_set;

    static __logfile_set m_logfile_set;

private:
    friend log_mgr &
    logger      ();
};

log_mgr &
logger ();

__NAMESPACE_END

#endif /* _LOGMGR_H_ */