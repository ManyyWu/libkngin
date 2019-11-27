#ifdef _WIN32
#else
#include <pthread.h>
#include "pthread.h"
#endif
#include "core/common.h"
#include "core/timestamp.h"
#include "core/rwlock.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/core/rwlock.cpp"

KNGIN_NAMESPACE_K_BEGIN

rwlock::rwlock ()
    : m_rwlock(PTHREAD_RWLOCK_INITIALIZER)
{
}

rwlock::~rwlock ()
{
    int _ret = ::pthread_rwlock_destroy(&m_rwlock);
    if (_ret)
        log_fatal("::pthread_rwlock_destroy() retturn %d", _ret);
}

void
rwlock::rdlock ()
{
    int _ret = ::pthread_rwlock_rdlock(&m_rwlock);
    if (_ret)
        log_fatal("::pthread_rwlock_rdlock() return %d", _ret);
}

void
rwlock::wrlock ()
{
    int _ret = ::pthread_rwlock_wrlock(&m_rwlock);
    if (_ret)
        log_fatal("::pthread_rwlock_wrlock() return %d", _ret);
}

bool
rwlock::tryrdlock ()
{
    int _ret = ::pthread_rwlock_tryrdlock(&m_rwlock);
    if (EBUSY == _ret)
        return false;
    if (_ret) {
        log_fatal("::pthread_rwlock_tryrdlock() return %d", _ret);
        return false;
    }
    return true;
}

bool
rwlock::trywrlock ()
{
    int _ret = ::pthread_rwlock_trywrlock(&m_rwlock);
    if (EBUSY == _ret)
        return false;
    if (_ret) {
        log_fatal("::pthread_rwlock_trywrlock() return %d", _ret);
        return false;
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
    int _ret = ::pthread_rwlock_timedrdlock(&m_rwlock, &_ts);
    if (ETIMEDOUT == _ret)
        return false;
    if (_ret) {
        log_fatal("::pthread_rwlock_timedrdlock(), return %d", _ret);
        return false;
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
    int _ret = ::pthread_rwlock_timedwrlock(&m_rwlock, &_ts);
    if (ETIMEDOUT == _ret)
        return false;
    if (_ret) {
        log_fatal("::pthread_rwlock_timedwrlock() return %d", _ret);
        return false;
    }

    return true;
}

void
rwlock::unlock ()
{
    int _ret = ::pthread_rwlock_unlock(&m_rwlock);
    if (_ret)
        log_fatal("::pthread_rwlock_unlock() return %d", _ret);
}

pthread_rwlock_t *
rwlock::get_interface ()
{
    return &m_rwlock;
}

KNGIN_NAMESPACE_K_END
