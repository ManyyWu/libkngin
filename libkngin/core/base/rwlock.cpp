#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include "core/base/common.h"
#include "core/base/timestamp.h"
#include "core/base/rwlock.h"
#include "core/base/exception.h"
#include "core/base/system_error.h"

#define KNGIN_FILENAME "libkngin/core/base/rwlock.cpp"

KNGIN_NAMESPACE_K_BEGIN

rwlock::rwlock ()
    : m_rwlock(PTHREAD_RWLOCK_INITIALIZER)
{
}

rwlock::~rwlock () KNGIN_NOEXCP
{
    ignore_excp(
        auto _ec = int2ec(::pthread_rwlock_destroy(&m_rwlock));
        if (_ec)
            log_fatal("::pthread_rwlock_destroy() error %s",
                      system_error_str(_ec).c_str());
    );
}

void
rwlock::rdlock ()
{
    auto _ec = int2ec(::pthread_rwlock_rdlock(&m_rwlock));
    if (_ec) {
        log_fatal("::pthread_rwlock_rdlock() error, %s",
                  system_error_str(_ec).c_str());
        throw k::exception("::pthread_rwlock_rdlock() error");
    }
}

void
rwlock::wrlock ()
{
    auto _ec = int2ec(::pthread_rwlock_wrlock(&m_rwlock));
    if (_ec) {
        log_fatal("::pthread_rwlock_wrlock() error, %s",
                  system_error_str(_ec).c_str());
        throw k::exception("::pthread_rwlock_wrlock() error");
    }
}

bool
rwlock::tryrdlock ()
{
    auto _ec = int2ec(::pthread_rwlock_tryrdlock(&m_rwlock));
    if (std::errc::device_or_resource_busy == _ec)
        return false;
    if (_ec) {
        log_fatal("::pthread_rwlock_tryrdlock() error, %s",
                  system_error_str(_ec).c_str());
        throw k::exception("::pthread_rwlock_tryrdlock() error");
    }
    return true;
}

bool
rwlock::trywrlock ()
{
    auto _ec = int2ec(::pthread_rwlock_trywrlock(&m_rwlock));
    if (std::errc::device_or_resource_busy == _ec)
        return false;
    if (_ec) {
        log_fatal("::pthread_rwlock_trywrlock() error, %s",
                  system_error_str(_ec).c_str());
        throw k::exception("::pthread_rwlock_trywrlock() error");
    }
    return true;
}

bool
rwlock::timedrdlock (timestamp _ms)
{
    timespec _ts;
    ::timespec_get(&_ts, TIME_UTC);
    timestamp _time = _ts;
    (_time += _ms).to_timespec(_ts);
    auto _ec = int2ec(::pthread_rwlock_timedrdlock(&m_rwlock, &_ts));
    if (std::errc::timed_out == _ec)
        return false;
    if (_ec) {
        log_fatal("::pthread_rwlock_timedrdlock(), error, %s",
                  system_error_str(_ec).c_str());
        throw k::exception("::pthread_rwlock_timedrdlock() error");
    }
    return true;
}

bool
rwlock::timedwrlock (timestamp _ms)
{
    timespec _ts;
    ::timespec_get(&_ts, TIME_UTC);
    timestamp _time = _ts;
    (_time += _ms).to_timespec(_ts);
    auto _ec = int2ec(::pthread_rwlock_timedwrlock(&m_rwlock, &_ts));
    if (std::errc::timed_out == _ec)
        return false;
    if (_ec) {
        log_fatal("::pthread_rwlock_timedwrlock() error, %d",
                  system_error_str(_ec).c_str());
        throw k::exception("::pthread_rwlock_timedwrlock() error");
    }
    return true;
}

void
rwlock::unlock ()
{
    auto _ec = int2ec(::pthread_rwlock_unlock(&m_rwlock));
    if (_ec) {
        log_fatal("::pthread_rwlock_unlock() return %d",
                  system_error_str(_ec).c_str());
        throw k::exception("::pthread_rwlock_unlock() error");
    }
}

KNGIN_NAMESPACE_K_END
