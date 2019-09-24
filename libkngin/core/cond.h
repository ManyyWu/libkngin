#ifndef _COND_H_
#define _COND_H_

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
#include "noncopyable.h"

__NAMESPACE_BEGIN

class cond : noncopyable {
public:
    cond () = delete;

    cond (cond &&_cond)
    {
        m_mutex = _cond.m_mutex;
        m_mutex = _cond.m_mutex;
    }
protected:
    inline
    cond (mutex *_mutex, pthread_cond_t *_cond_intr)
        : m_mutex(_mutex), m_cond(_cond_intr)
    {
        kassert(_mutex);
        kassert(_cond_intr);
    }

protected:
    inline
    ~cond ()
    {
        if (m_cond) {
            int _ret = 0;
            _ret = pthread_cond_destroy(m_cond);
            if_not (!_ret)
                server_fatal("pthread_cond_destroy() return %d", _ret);
            safe_release(m_cond);
        }
    }

public:
    inline static cond *
    create (mutex *_mutex)
    {
        kassert_r0(_mutex);

        int _ret = 0;
        cond *          _cond = NULL;
        pthread_cond_t *_cond_intr = NULL;
        _cond_intr = new_nothrow(pthread_cond_t);
        if_not (_cond_intr)
            goto fail;
        _ret = pthread_cond_init(_cond_intr, NULL);
        if_not (!_ret) {
            server_fatal("pthread_cond_init() return %d", _ret);
            goto fail;
        }
        _cond = new_nothrow(cond(_mutex, _cond_intr));
        if_not (_cond)
            goto fail;
        return _cond;
fail:
        if (_cond_intr)
            pthread_cond_destroy(_cond_intr);
        safe_release(_cond_intr);
        safe_release(_cond);
        return NULL;
    }

    inline void
    release ()
    {
        kassert(m_cond);

        delete this;
    }

public:
    inline bool
    wait ()
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

    inline bool
    timedwait (time_t _ms)
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

    inline bool
    signal ()
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


    inline bool
    broadcast ()
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
public:
    inline pthread_cond_t *
    get_interface () const
    {
        return m_cond;
    }

protected:
    pthread_cond_t *m_cond;

    mutex *         m_mutex;
};

__NAMESPACE_END

#endif /* _COND_H_ */