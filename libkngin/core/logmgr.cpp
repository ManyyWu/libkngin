#include <exception>
#include "log.h"
#include "logmgr.h"
#include "define.h"

__NAMESPACE_BEGIN

__logfile_set log_mgr::m_logfile_set = {
    "kngin_server", "kngin_http"
};

__log_set     log_mgr::m_log_set;

log_mgr::log_mgr ()
{
}

log_mgr::~log_mgr ()
{
    if (!log_mgr::m_log_set.empty())
        for (auto _iter : log_mgr::m_log_set)
            delete _iter;
}

const log &
log_mgr::operator [] (int _index) const
{
    assert(_index >= 0 && _index < log_mgr::m_log_set.size());
    return *log_mgr::m_log_set.at(_index);
}

const log &
log_mgr::at (int _index) const
{
    assert(_index >= 0 && _index < log_mgr::m_log_set.size());
    return *log_mgr::m_log_set.at(_index);
}

std::string &
log_mgr::filename_at (int _index) const
{
    assert(_index >= 0 && _index < log_mgr::m_log_set.size());
    return log_mgr::m_logfile_set.at(_index);
}

log_mgr &
logger ()
{
///// test ///// 
// read log config from config file
    static log_mgr _logger;
    if (log_mgr::m_log_set.empty()) {
        log *_log1 = new_nothrow(log(__LOG_FILE_SERVER, __LOG_MODE_BOTH));
        log *_log2 = new_nothrow(log(__LOG_FILE_SERVER, __LOG_MODE_BOTH));
        assert(_log1);
        assert(_log2);
        _log1->init();
        _log2->init();
        log_mgr::m_log_set.push_back(_log1);
        log_mgr::m_log_set.push_back(_log2);
    }
///// test ///// 

    return _logger;
}

__NAMESPACE_END
