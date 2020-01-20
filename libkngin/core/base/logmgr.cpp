#include <iostream>
#include <cassert>
#include <exception>
#include "core/base/log.h"
#include "core/base/logmgr.h"
#include "core/base/common.h"

KNGIN_NAMESPACE_K_BEGIN

std::atomic_bool           log_mgr::m_inited(false);

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
        m_log_set.push_back(
            std::make_unique<log>(KNGIN_LOG_FILE_SERVER, KNGIN_LOG_MODE_STDERR));
    } catch (std::exception &_e) {
        std::cerr << "logger init failed: " << _e.what() << std::endl;
        throw;
    } catch (...) {
        std::cerr << "logger init failed" << std::endl;
        throw;
    }
    m_inited = true;
}

log_mgr::~log_mgr () KNGIN_NOEXCP
{
    m_inited = false;
}

log &
log_mgr::operator [] (size_t _index) KNGIN_NOEXCP
{
    assert(log_mgr::m_inited);
    assert(log_mgr::m_log_set.size() == log_mgr::m_logfile_set.size());
    return *log_mgr::m_log_set[_index];
}

const std::string &
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
