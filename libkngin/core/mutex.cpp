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

mutex::mutex (pthread_mutex_t *_mutex_intr)
    : m_mutex(_mutex_intr)
{
    kassert(_mutex_intr);
}

mutex::~mutex ()
{
    if (m_mutex) {
        int _ret = 0;
        _ret = pthread_mutex_destroy(m_mutex);
        if_not (!_ret)
            server_fatal("pthread_mutex_destroy() return %d", _ret);
        kdelete(m_mutex);
    }
}

mutex *
mutex::create ()
{
    int _ret = 0;
    mutex * _mutex = NULL;
    pthread_mutex_t *_mutex_intr = NULL;

    knew(_mutex_intr, pthread_mutex_t, ());
    if_not (_mutex_intr)
        return NULL;
    _ret = pthread_mutex_init(_mutex_intr, NULL);
    if_not (!_ret) {
        server_fatal("pthread_mutex_init() return %d", _ret);
        goto fail;
    }
    knew(_mutex, mutex, (_mutex_intr));
    if_not (_mutex)
        goto fail;
    return _mutex;
    fail:
    if (_mutex_intr)
        pthread_mutex_destroy(_mutex_intr);
    kdelete(_mutex_intr);
    kdelete(_mutex);
    return NULL;
}

void
mutex::release ()
{
    kdelete_this(this);
}

bool
mutex::lock ()
{
    kassert_r0(m_mutex);

    int _ret = 0;
    _ret = pthread_mutex_lock(m_mutex);
    if_not (!_ret) {
        server_fatal("pthread_mutex_lock() return %d", _ret);
        return false;
    }
    return true;
}

bool
mutex::trylock ()
{
    kassert_r0(m_mutex);

    int _ret = 0;
    _ret = pthread_mutex_trylock(m_mutex);
    if (EBUSY == _ret)
        return false;
    if_not (!_ret) {
        server_fatal("pthread_mutex_trylock() return %d", _ret);
        return false;
    }
    return true;
}

bool
mutex::timedlock (time_t _ms)
{
    kassert_r0(m_mutex);
    kassert_r0(__time_valid(_ms));
    if (!__time_valid(_ms))
        return false;

    int _ret = 0;
    timespec _ts;
    timespec_get(&_ts, TIME_UTC);
    _ts.tv_sec += _ms / 1000;
    _ts.tv_nsec += (_ms % 1000) * 1000000;
    _ret = pthread_mutex_timedlock(m_mutex, &_ts);
    if (ETIMEDOUT == _ret)
        return false;
    if_not (!_ret) {
        server_fatal("pthread_mutex_timedlock(), value = %ld, return %d", _ms, _ret);
        return false;
    }
    return true;
}

bool
mutex::unlock ()
{
    kassert_r0(m_mutex);

    int _ret = 0;
    _ret = pthread_mutex_unlock(m_mutex);
    if (ETIMEDOUT == _ret)
        return false;
    if_not (!_ret) {
        server_fatal("pthread_mutex_unlock() return %d", _ret);
        return false;
    }
    return true;
}

const pthread_mutex_t *
mutex::get_interface () const
{
    return m_mutex;
}

__NAMESPACE_END
