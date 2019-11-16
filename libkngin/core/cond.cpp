#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include "common.h"
#include "timestamp.h"
#include "mutex.h"
#include "cond.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin/core/cond.cpp"

__NAMESPACE_BEGIN

cond::cond (mutex *_mutex)
    : m_cond(PTHREAD_COND_INITIALIZER), m_mutex(_mutex)
{
    check(_mutex);
    int _ret = ::pthread_cond_init(&m_cond, nullptr);
    if (_ret) {
        log_fatal("::pthread_cond_init() return %d", _ret);
        throw exception("cond::cond() error");
    }
}

cond::~cond ()
{
    int _ret = ::pthread_cond_destroy(&m_cond);
    if (_ret)
        log_fatal("::pthread_cond_destroy() return %d", _ret);
}

void
cond::wait ()
{
    int _ret = ::pthread_cond_wait(&m_cond, &m_mutex->m_mutex);
    if (_ret)
        log_fatal("::pthread_cond_wait() return %d", _ret);
}

bool
cond::timedwait (timestamp _ms)
{
    timespec _ts;
    ::timespec_get(&_ts, TIME_UTC);
    timestamp _time = _ts;
    (_time += _ms).to_timespec(_ts);
    int _ret = ::pthread_cond_timedwait(&m_cond, &m_mutex->m_mutex, &_ts);
    if (ETIMEDOUT == _ret)
        return false;
    if (_ret) {
        log_fatal("::pthread_cond_timedwait() return %d", _ret);
        return false;
    }
    return true;
}

void
cond::signal ()
{
    int _ret = ::pthread_cond_signal(&m_cond);
    if (_ret)
        log_fatal("::pthread_cond_signal() return %d", _ret);
}


void
cond::broadcast ()
{
    int _ret = ::pthread_cond_broadcast(&m_cond);
    if (_ret)
        log_fatal("::pthread_cond_broadcast() return %d", _ret);
}

pthread_cond_t *
cond::get_interface ()
{
    return &m_cond;
}

__NAMESPACE_END
