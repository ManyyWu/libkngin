#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include "time.h"
#include "error.h"
#include "define.h"
#include "logfile.h"
#include "common.h"
#include "thread.h"
#include "mutex.h"
#include "cond.h"
#include "noncopyable.h"

__NAMESPACE_BEGIN

cond::cond (cond &&_cond)
{
    m_mutex = _cond.m_mutex;
    m_mutex = _cond.m_mutex;
}

cond::cond (mutex *_mutex, pthread_cond_t *_cond_intr)
    : m_mutex(_mutex), m_cond(_cond_intr)
{
    kassert(_mutex);
    kassert(_cond_intr);
}

cond::~cond ()
{
    if (m_cond) {
        int _ret = 0;
        _ret = pthread_cond_destroy(m_cond);
        if_not (!_ret)
            server_fatal("pthread_cond_destroy() return %d", _ret);
        kdelete(m_cond);
    }
}

cond *
cond::create (mutex *_mutex)
{
    kassert_r0(_mutex);

    int _ret = 0;
    cond *          _cond = NULL;
    pthread_cond_t *_cond_intr = NULL;
    knew(_cond_intr, pthread_cond_t, ());
    if_not (_cond_intr)
        goto fail;
    _ret = pthread_cond_init(_cond_intr, NULL);
    if_not (!_ret) {
        server_fatal("pthread_cond_init() return %d", _ret);
        goto fail;
    }
    knew(_cond, cond, (_mutex, _cond_intr));
    if_not (_cond)
        goto fail;
    return _cond;
    fail:
    if (_cond_intr)
        pthread_cond_destroy(_cond_intr);
    kdelete(_cond_intr);
    kdelete(_cond);
    return NULL;
}

void
cond::release ()
{
    kdelete_this(this);
}

bool
cond::wait ()
{
    kassert_r0(m_cond);

    int _ret = 0;
    _ret = pthread_cond_wait(m_cond, m_mutex->m_mutex);
    if_not (!_ret) {
        server_fatal("pthread_cond_wait() return %d", _ret);
        return false;
    }
    return true;
}

bool
cond::timedwait (time_t _ms)
{
    kassert_r0(m_cond);
    kassert_r0(__time_valid(_ms));

    int _ret = 0;
    timespec _ts;
    timespec_get(&_ts, TIME_UTC);
    _ts.tv_sec += _ms / 1000;
    _ts.tv_nsec += (_ms % 1000) * 1000000;
    _ret = pthread_cond_timedwait(m_cond, m_mutex->m_mutex, &_ts);
    if (ETIMEDOUT == _ret)
        return false;
    if_not (!_ret) {
        server_fatal("pthread_cond_timedwait() return %d", _ret);
        return false;
    }
    return true;
}

bool
cond::signal ()
{
    kassert_r0(m_cond);

    int _ret = 0;
    _ret = pthread_cond_signal(m_cond);
    if_not (!_ret) {
        server_fatal("pthread_cond_signal() return %d", _ret);
        return false;
    }
    return true;
}


bool
cond::broadcast ()
{
    kassert_r0(m_cond);

    int _ret = 0;
    _ret = pthread_cond_broadcast(m_cond);
    if_not (!_ret) {
        server_fatal("pthread_cond_broadcast() return %d", _ret);
        return false;
    }
    return true;
}

pthread_cond_t *
cond::get_interface () const
{
    return m_cond;
}

__NAMESPACE_END
