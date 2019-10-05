#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include "define.h"
#include "logfile.h"
#include "common.h"
#include "thread.h"

__NAMESPACE_BEGIN

mutex::mutex ()
    : m_mutex(PTHREAD_MUTEX_INITIALIZER)
{
    int _ret = ::pthread_mutex_init(&m_mutex, NULL);
    if_not (!_ret) {
        log_fatal("::pthread_mutex_init() return %d", _ret);
        throw exception("mutex::mutex() error");
    }
}

mutex::~mutex ()
{
    int _ret = ::pthread_mutex_destroy(&m_mutex);
    if_not (!_ret)
        log_fatal("::pthread_mutex_destroy() return %d", _ret);
}

void
mutex::lock ()
{
    int _ret = ::pthread_mutex_lock(&m_mutex);
    if_not (!_ret)
        log_fatal("::pthread_mutex_lock() return %d", _ret);
}

bool
mutex::trylock ()
{
    int _ret = ::pthread_mutex_trylock(&m_mutex);
    if (EBUSY == _ret)
        return false;
    if_not (!_ret) {
        log_fatal("pthread_mutex_trylock() return %d", _ret);
        return false;
    }
    return true;
}

bool
mutex::timedlock (time_t _ms)
{
    if_not (__time_valid(_ms))
        return false;

    timespec _ts;
    ::timespec_get(&_ts, TIME_UTC);
    _ts.tv_sec += _ms / 1000;
    _ts.tv_nsec += (_ms % 1000) * 1000000;
    int _ret = ::pthread_mutex_timedlock(&m_mutex, &_ts);
    if (ETIMEDOUT == _ret)
        return false;
    if_not (!_ret) {
        log_fatal("::pthread_mutex_timedlock(), value = %ld, return %d", _ms, _ret);
        return false;
    }
    return true;
}

void
mutex::unlock ()
{
    int _ret = ::pthread_mutex_unlock(&m_mutex);
    if_not (!_ret)
        log_fatal("::pthread_mutex_unlock() return %d", _ret);
}

pthread_mutex_t *
mutex::get_interface ()
{
    return &m_mutex;
}

__NAMESPACE_END
