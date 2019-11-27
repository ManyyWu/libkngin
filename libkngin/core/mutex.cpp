#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include "core/common.h"
#include "core/timestamp.h"
#include "core/mutex.h"

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

KNGIN_NAMESPACE_K_END
