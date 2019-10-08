#include <exception>
#include "define.h"
#include "error.h"
#include "log.h"
#include "logmgr.h"
#include "common.h"

__NAMESPACE_BEGIN

__log_set         log_mgr::m_log_set;
std::atomic<bool> log_mgr::m_inited(false);
__logfile_set log_mgr::m_logfile_set = {
    "kngin_memory",
    "kngin_server",
    "kngin_http"
};

log_mgr::log_mgr ()
{
}

log_mgr::~log_mgr ()
{
    // log
    m_inited.store(false);
    while (log_mgr::m_log_set.size()) {
        log *_first = *log_mgr::m_log_set.begin();
        log_mgr::m_log_set.erase(log_mgr::m_log_set.begin());
        delete _first;
    }

    __log_set _temp;
    m_log_set.swap(_temp);
    __logfile_set _temp1;
    m_logfile_set.swap(_temp1);
}

log *
log_mgr::operator [] (size_t _index)
{
    if (log_mgr::m_log_set.size() != log_mgr::m_logfile_set.size())
        return NULL;
    return log_mgr::m_log_set.at(_index);
}

log *
log_mgr::at (size_t _index)
{
    if (log_mgr::m_log_set.size() != log_mgr::m_logfile_set.size())
        return NULL;
    return log_mgr::m_log_set.at(_index);
}

std::string &
log_mgr::filename_at (size_t _index)
{
    assert(_index < log_mgr::m_log_set.size());
    return log_mgr::m_logfile_set.at(_index);
}

bool
log_mgr::inited () const
{
    return log_mgr::m_inited.load();
}

log_mgr &
logger ()
{
///// test ///// 
// read log config from config file
// can't write any log before inited
    static log_mgr _logger;
    static bool _init = true;
    if (log_mgr::m_log_set.empty() && _init) {
        _init = false;
        log *_log1 = new(std::nothrow) log(__LOG_FILE_MEMORY, __LOG_MODE_BOTH);
        log *_log2 = new(std::nothrow) log(__LOG_FILE_SERVER, __LOG_MODE_BOTH);
        log *_log3 = new(std::nothrow) log(__LOG_FILE_HTTP, __LOG_MODE_BOTH);
        if (!_log1 || !_log2 || !_log3) {
            // log
fail: 
            delete _log1;
            delete _log2;
            delete _log3;
            __log_set _temp;
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
        log_mgr::m_inited.store(true);
        // log
    }

    return _logger;
///// test /////
}

__NAMESPACE_END
