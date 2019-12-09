#include <iostream>
#include <cassert>
#include "core/exception.h"
#include "core/log.h"
#include "core/logmgr.h"
#include "core/common.h"

KNGIN_NAMESPACE_K_BEGIN

std::atomic<bool>          log_mgr::m_inited(false);

log_mgr::kngin_log_set     log_mgr::m_log_set;

log_mgr::kngin_logfile_set log_mgr::m_logfile_set = {
    "kngin_server", // default
};

log_mgr::log_mgr ()
{
    if (log_mgr::m_inited)
        return;

    try {
        // reserved type
        log *_server_log = new log(KNGIN_LOG_FILE_SERVER, KNGIN_LOG_MODE_STDERR);
        try {
            m_log_set.push_back(_server_log);
        } catch (...) {
            safe_release(_server_log);
            throw;
        }
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

log_mgr::~log_mgr () KNGIN_NOEXCP
{
    m_inited = false;
    kngin_log_set _temp;
    m_log_set.swap(_temp);
    kngin_logfile_set _temp1;
    m_logfile_set.swap(_temp1);
}

log &
log_mgr::operator [] (size_t _index) KNGIN_NOEXCP
{
    assert(log_mgr::m_inited);
    assert(log_mgr::m_log_set.size() == log_mgr::m_logfile_set.size());
    return *log_mgr::m_log_set[_index];
}

int
log_mgr::add (const std::string &_filename, KNGIN_LOG_MODE _mode)
{
    assert(log_mgr::m_inited);
    m_logfile_set.push_back(_filename);
    int _index = m_log_set.size();
    log *_new_log = new log(static_cast<KNGIN_LOG_FILE>(_index), _mode);
    try {
        m_log_set.push_back(_new_log);
    } catch (...) {
        safe_release(_new_log);
        throw;
    }

    return _index;
}

std::string &
log_mgr::filename_at (size_t _index) KNGIN_NOEXCP
{
    assert(log_mgr::m_inited);
    assert(_index < log_mgr::m_log_set.size());
    return log_mgr::m_logfile_set[_index];
}

bool
log_mgr::inited () const KNGIN_NOEXCP
{
    return log_mgr::m_inited;
}

log_mgr &
logger ()
{
    static log_mgr _logger;

    return _logger;
}

KNGIN_NAMESPACE_K_END
