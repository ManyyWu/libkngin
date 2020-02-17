#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include "core/base/common.h"
#include "core/base/timestamp.h"
#include "core/base/mutex.h"
#include "core/base/exception.h"
#include "core/base/system_error.h"

#define KNGIN_FILENAME "libkngin/core/base/mutex.cpp"

KNGIN_NAMESPACE_K_BEGIN

mutex::mutex ()
    : m_mutex(PTHREAD_MUTEX_INITIALIZER)
{
}

mutex::~mutex ()
{
    ignore_excp(
        auto _ec = ::pthread_mutex_destroy(&m_mutex);
        if (_ec)
            log_fatal("::pthread_mutex_destroy() error, %s",
                      system_error_str(_ec).c_str());
    );
}

void
mutex::lock ()
{
    auto _ec = ::pthread_mutex_lock(&m_mutex);
    if (_ec) {
        log_fatal("::pthread_mutex_lock() error, %s",
                  system_error_str(_ec).c_str());
        throw k::exception("::pthread_mutex_lock() error");
    }
}

bool
mutex::trylock ()
{
    auto _ec = ::pthread_mutex_trylock(&m_mutex);
    if (EBUSY == _ec)
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
    auto _ec = ::pthread_mutex_timedlock(&m_mutex, &_ts);
    if (ETIMEDOUT == _ec)
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
    auto _ec = ::pthread_mutex_unlock(&m_mutex);
    if (_ec) {
        log_fatal("::pthread_mutex_unlock() error, %s",
                  system_error_str(_ec).c_str());
        throw k::exception("::pthread_mutex_unlock() error");
    }
}

KNGIN_NAMESPACE_K_END
