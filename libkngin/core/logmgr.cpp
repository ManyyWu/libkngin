#include <exception>
#include "define.h"
#include "error.h"
#include "log.h"
#include "logmgr.h"
#include "common.h"

__NAMESPACE_BEGIN

log_mgr::__logfile_set log_mgr::m_logfile_set = {
    "kngin_memory",
    "kngin_server",
    "kngin_http"
};

log_mgr::__log_set log_mgr::m_log_set;

log_mgr::log_mgr ()
{
}

log_mgr::~log_mgr ()
{
    if (!log_mgr::m_log_set.empty())
        for (auto _iter : log_mgr::m_log_set)
            delete _iter;
    __log_set _temp;
    m_log_set.swap(_temp);
}

log *
log_mgr::operator [] (size_t _index)
{
    assert(_index >= 0 && _index < log_mgr::m_log_set.size());
    return log_mgr::m_log_set.at(_index);
}

log *
log_mgr::at (size_t _index)
{
    assert(_index >= 0 && _index < log_mgr::m_log_set.size());
    return log_mgr::m_log_set.at(_index);
}

std::string &
log_mgr::filename_at (size_t _index)
{
    assert(_index < log_mgr::m_log_set.size());
    return log_mgr::m_logfile_set.at(_index);
}

log_mgr &
logger ()
{
///// test ///// 
// read log config from config file
    static log_mgr _logger;
    if (log_mgr::m_log_set.empty()) {
        log *_log1 = new(std::nothrow) log(__LOG_FILE_MEMORY, __LOG_MODE_BOTH);
        log *_log2 = new(std::nothrow) log(__LOG_FILE_SERVER, __LOG_MODE_BOTH);
        log *_log3 = new(std::nothrow) log(__LOG_FILE_HTTP, __LOG_MODE_BOTH);
        if (!_log1 || !_log2 || !_log3) {
fail: 
            delete _log1;
            delete _log2;
            delete _log3;
            log_mgr::__log_set _temp;
            log_mgr::m_log_set.swap(_temp);
            exit(E_SERVER_INIT_FAIL);
        }
        log_mgr::m_log_set.push_back(_log1);
        log_mgr::m_log_set.push_back(_log2);
        log_mgr::m_log_set.push_back(_log3);

        if (!_log1->init() || !_log2->init() || !_log3->init()) {
            goto fail;
        }
        _log1 = NULL;
        _log2 = NULL;
        _log3 = NULL;
    }

    return _logger;
///// test /////
}

__NAMESPACE_END
