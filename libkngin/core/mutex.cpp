#ifdef _WIN32
#include "pthreads_win32/include/pthread.h"
#else
#include <pthread.h>
#endif
#include "core/common.h"
#include "core/timestamp.h"
#include "core/mutex.h"
#include "core/exception.h"
#include "core/system_error.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/core/mutex.cpp"

KNGIN_NAMESPACE_K_BEGIN

mutex::mutex ()
    : m_mutex(PTHREAD_MUTEX_INITIALIZER)
{
}

mutex::~mutex ()
{
    ignore_excp(
        std::error_code _ec = int2ec(::pthread_mutex_destroy(&m_mutex));
        if (_ec)
            log_fatal("::pthread_mutex_destroy() error, %s",
                      system_error_str(_ec).c_str());
    );
}

void
mutex::lock ()
{
    std::error_code _ec = int2ec(::pthread_mutex_lock(&m_mutex));
    if (_ec) {
        log_fatal("::pthread_mutex_lock() error, %s",
                  system_error_str(_ec).c_str());
        throw k::exception("::pthread_mutex_lock() error");
    }
}

bool
mutex::trylock ()
{
    std::error_code _ec = int2ec(::pthread_mutex_trylock(&m_mutex));
    if (std::errc::device_or_resource_busy == _ec)
        return false;
    if (_ec) {
        log_fatal("::pthread_mutex_trylock() error, %s",
                  system_error_str(_ec).c_str());
        throw k::exception("::pthread_mutex_trylock() error");
    }
    return true;
}

bool
mutex::timedlock (timestamp _ms)
{
    timespec _ts;
    ::timespec_get(&_ts, TIME_UTC);
    timestamp _time = _ts;
    (_time += _ms).to_timespec(_ts);
    std::error_code _ec = int2ec(::pthread_mutex_timedlock(&m_mutex, &_ts));
    if (std::errc::timed_out == _ec)
        return false;
    if (_ec) {
        log_fatal("::pthread_mutex_timedlock() error, %s",
                  system_error_str(_ec).c_str());
        throw k::exception("::pthread_mutex_timedlock() error");
    }
    return true;
}

void
mutex::unlock ()
{
    std::error_code _ec = int2ec(::pthread_mutex_unlock(&m_mutex));
    if (_ec) {
        log_fatal("::pthread_mutex_unlock() error, %s",
                  system_error_str(_ec).c_str());
        throw k::exception("::pthread_mutex_unlock() error");
    }
}

KNGIN_NAMESPACE_K_END
