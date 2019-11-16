#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include "common.h"
#include "timestamp.h"
#include "mutex.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin/core/mutex.cpp"

__NAMESPACE_BEGIN

mutex::mutex ()
    : m_mutex(PTHREAD_MUTEX_INITIALIZER)
{
    int _ret = ::pthread_mutex_init(&m_mutex, nullptr);
    if (_ret) {
        log_fatal("::pthread_mutex_init() return %d", _ret);
        throw exception("mutex::mutex() error");
    }
}

mutex::~mutex ()
{
    int _ret = ::pthread_mutex_destroy(&m_mutex);
    if (_ret)
        log_fatal("::pthread_mutex_destroy() return %d", _ret);
}

void
mutex::lock ()
{
    int _ret = ::pthread_mutex_lock(&m_mutex);
    if (_ret)
        log_fatal("::pthread_mutex_lock() return %d", _ret);
}

bool
mutex::trylock ()
{
    int _ret = ::pthread_mutex_trylock(&m_mutex);
    if (EBUSY == _ret)
        return false;
    if (_ret) {
        log_fatal("pthread_mutex_trylock() return %d", _ret);
        return false;
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
    int _ret = ::pthread_mutex_timedlock(&m_mutex, &_ts);
    if (ETIMEDOUT == _ret)
        return false;
    if (_ret) {
        log_fatal("::pthread_mutex_timedlock() return %d", _ret);
        return false;
    }
    return true;
}

void
mutex::unlock ()
{
    int _ret = ::pthread_mutex_unlock(&m_mutex);
    if (_ret)
        log_fatal("::pthread_mutex_unlock() return %d", _ret);
}

pthread_mutex_t *
mutex::get_interface ()
{
    return &m_mutex;
}

__NAMESPACE_END
