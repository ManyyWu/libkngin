#include <cassert>
#include "define.h"
#include "error.h"
#include "exception.h"
#include "log.h"
#include "logmgr.h"
#include "common.h"

__NAMESPACE_BEGIN

std::atomic<bool>       log_mgr::m_inited(false);

log_mgr::__log_set     log_mgr::m_log_set;

log_mgr::__logfile_set log_mgr::m_logfile_set = {
    "kngin_memory",
    "kngin_server", // default
};

log_mgr::log_mgr ()
{
    if (log_mgr::m_inited)
        return;

    try {
        // reserved type
        m_log_set.push_back(new log(__LOG_FILE_MEMORY, __LOG_MODE_FILE));
        m_log_set.push_back(new log(__LOG_FILE_SERVER, __LOG_MODE_BOTH));
        log_mgr::m_inited = true;
    } catch (...) {
        for (auto _iter : m_log_set) {
            delete _iter;
            log_mgr::m_log_set.pop_back();
        }
        std::cerr << "logger init failed" << std::endl;
        throw;
    }
}

log_mgr::~log_mgr ()
{
    m_inited = false;
    __log_set _temp;
    m_log_set.swap(_temp);
    __logfile_set _temp1;
    m_logfile_set.swap(_temp1);
}

log &
log_mgr::operator [] (size_t _index)
{
    assert(log_mgr::m_inited);
    assert(log_mgr::m_log_set.size() == log_mgr::m_logfile_set.size());
    return *log_mgr::m_log_set[_index];
}

int
log_mgr::add (const std::string &_filename, __LOG_MODE _mode)
{
    assert(log_mgr::m_inited);
    m_logfile_set.push_back(_filename);
    int _index = m_log_set.size();
    m_log_set.push_back(new log((__LOG_FILE)_index, _mode));
    return _index;
}

std::string &
log_mgr::filename_at (size_t _index)
{
    assert(log_mgr::m_inited);
    assert(_index < log_mgr::m_log_set.size());
    return log_mgr::m_logfile_set[_index];
}

bool
log_mgr::inited () const
{
    return log_mgr::m_inited;
}

log_mgr &
logger ()
{
    static log_mgr _logger;

    return _logger;
}

__NAMESPACE_END
