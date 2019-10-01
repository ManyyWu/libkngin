#ifdef _WIN32
#else
#include <pthread.h>
#include "pthread.h"
#endif
#include "define.h"
#include "logfile.h"
#include "common.h"
#include "thread.h"
#include "rwlock.h"

__NAMESPACE_BEGIN

rwlock::rwlock ()
    : m_rwlock(PTHREAD_RWLOCK_INITIALIZER)
{
    int _ret = pthread_rwlock_init(&m_rwlock, NULL);
    if_not (!_ret)
        log_fatal("pthread_rwlock_init() return %d", _ret);
}

rwlock::~rwlock ()
{
    int _ret = pthread_rwlock_destroy(&m_rwlock);
    if_not (!_ret)
        log_fatal("pthread_rwlock_destroy() retturn %d", _ret);
}

void
rwlock::rdlock ()
{
    int _ret = pthread_rwlock_rdlock(&m_rwlock);
    if_not (!_ret)
        log_fatal("pthread_rwlock_rdlock() return %d", _ret);
}

void
rwlock::wrlock ()
{
    int _ret = pthread_rwlock_wrlock(&m_rwlock);
    if_not (!_ret)
        log_fatal("pthread_rwlock_wrlock() return %d", _ret);
}

bool
rwlock::tryrdlock ()
{
    int _ret = pthread_rwlock_tryrdlock(&m_rwlock);
    if (EBUSY == _ret)
        return false;
    if_not (!_ret) {
        log_fatal("pthread_rwlock_tryrdlock() return %d", _ret);
        return false;
    }
    return true;
}

bool
rwlock::trywrlock ()
{
    int _ret = pthread_rwlock_trywrlock(&m_rwlock);
    if (EBUSY == _ret)
        return false;
    if_not (!_ret) {
        log_fatal("pthread_rwlock_trywrlock() return %d", _ret);
        return false;
    }
    return true;
}

bool
rwlock::timedrdlock (time_t _ms)
{
    if_not (__time_valid(_ms))
        return false;

    timespec _ts;
    timespec_get(&_ts, TIME_UTC);
    _ts.tv_sec += _ms / 1000;
    _ts.tv_nsec += (_ms % 1000) * 1000000;
    int _ret = pthread_rwlock_timedrdlock(&m_rwlock, &_ts);
    if (ETIMEDOUT == _ret)
        return false;
    if_not (!_ret) {
        log_fatal("pthread_rwlock_timedrdlock(), value = %ld, return %d", _ms, _ret);
        return false;
    }

    return true;
}

bool
rwlock::timedwrlock (time_t _ms)
{
    if_not (__time_valid(_ms))
        return false;

    timespec _ts;
    timespec_get(&_ts, TIME_UTC);
    _ts.tv_sec += _ms / 1000;
    _ts.tv_nsec += (_ms % 1000) * 1000000;
    int _ret = pthread_rwlock_timedwrlock(&m_rwlock, &_ts);
    if (ETIMEDOUT == _ret)
        return false;
    if_not (!_ret) {
        log_fatal("pthread_rwlock_timedwrlock(), value = %ld, return %d", _ms, _ret);
        return false;
    }

    return true;
}

void
rwlock::unlock ()
{
    int _ret = pthread_rwlock_unlock(&m_rwlock);
    if_not (!_ret)
        log_fatal("pthread_rwlock_unlock() return %d", _ret);
}

pthread_rwlock_t *
rwlock::get_interface ()
{
    return &m_rwlock;
}

__NAMESPACE_END
