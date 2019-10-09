#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include "timestamp.h"
#include "error.h"
#include "define.h"
#include "logfile.h"
#include "common.h"
#include "thread.h"
#include "mutex.h"
#include "cond.h"
#include "noncopyable.h"

__NAMESPACE_BEGIN

cond::cond (mutex *_mutex)
    : m_cond(PTHREAD_COND_INITIALIZER), m_mutex(_mutex)
{
    kassert(_mutex);
    int _ret = ::pthread_cond_init(&m_cond, NULL);
    if (_ret) {
        log_fatal("::pthread_cond_init() return %d", _ret);
        throw exception("cond::cond() error");
    }
}

cond::~cond ()
{
    int _ret = ::pthread_cond_destroy(&m_cond);
    if_not (!_ret)
        log_fatal("::pthread_cond_destroy() return %d", _ret);
}

    void
cond::wait ()
{
    int _ret = ::pthread_cond_wait(&m_cond, &m_mutex->m_mutex);
    if_not (!_ret)
        log_fatal("::pthread_cond_wait() return %d", _ret);
}

bool
cond::timedwait (time_t _ms)
{
    if_not (__time_valid(_ms))
        return false;

    timespec _ts;
    ::timespec_get(&_ts, TIME_UTC);
    _ts.tv_sec += _ms / 1000;
    _ts.tv_nsec += (_ms % 1000) * 1000000;
    int _ret = ::pthread_cond_timedwait(&m_cond, &m_mutex->m_mutex, &_ts);
    if (ETIMEDOUT == _ret)
        return false;
    if_not (!_ret) {
        log_fatal("::pthread_cond_timedwait() return %d", _ret);
        return false;
    }
    return true;
}

void
cond::signal ()
{
    int _ret = ::pthread_cond_signal(&m_cond);
    if_not (!_ret)
        log_fatal("::pthread_cond_signal() return %d", _ret);
}


void
cond::broadcast ()
{
    int _ret = ::pthread_cond_broadcast(&m_cond);
    if_not (!_ret)
        log_fatal("::pthread_cond_broadcast() return %d", _ret);
}

pthread_cond_t *
cond::get_interface ()
{
    return &m_cond;
}

__NAMESPACE_END
