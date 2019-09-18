#ifndef _COND_H_
#define _COND_H_

#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
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

protected:
    inline
    cond (mutex *_mutex, pthread_cond_t *_cond_intr)
        : m_mutex(_mutex), m_cond(_cond_intr)
    {
        assert(_mutex);
        assert(_cond_intr);
    }

protected:
    inline
    ~cond ()
    {
        int _ret = 0;

        if (m_cond) {
            _ret = pthread_cond_destroy(m_cond);
            delete m_cond;
        }
        assert(!_ret);
    }

public:
    inline static cond *
    create (mutex *_mutex)
    {
        assert(_mutex);

        int _ret = 0;
        cond *          _cond = NULL;
        pthread_cond_t *_cond_intr = NULL;
        _cond_intr = new pthread_cond_t;
        if (!_cond_intr)
            goto fail;
        _ret = pthread_cond_init(_cond_intr, NULL);
        if (_ret)
            goto fail;
        _cond = new cond(_mutex, _cond_intr);
        if (!_cond)
            goto fail;
        return _cond;
fail:
        if (_cond_intr)
            pthread_cond_destroy(_cond_intr);
        delete _cond_intr;
        delete _cond;
        return NULL;
    }

    inline void
    release ()
    {
        assert(m_cond);

        delete this;
    }

public:
    inline bool
    wait ()
    {
        assert(m_cond);

        int _ret = 0;
        assert(m_cond);
        _ret = pthread_cond_wait(m_cond, m_mutex->m_mutex);
        assert(!_ret);
        if (_ret)
            return false;
        return true;
    }

    inline bool
    timedwait (int _ms)
    {
        assert(m_cond);

        int _ret = 0;
        timespec _ts;
        _ts.tv_sec = _ms / 1000;
        _ts.tv_nsec = (_ms % 1000) * 1000000;
        _ret = pthread_cond_timedwait(m_cond, m_mutex->m_mutex, &_ts);
        assert(!_ret);
        if (_ret)
            return false;
        return true;
    }

    inline bool
    signal ()
    {
        assert(m_cond);

        int _ret = 0;
        assert(m_cond);
        _ret = pthread_cond_signal(m_cond);
        assert(!_ret);
        if (_ret)
            return false;
        return true;
    }


    inline bool
    broadcast ()
    {
        assert(m_cond);

        int _ret = 0;
        _ret = pthread_cond_broadcast(m_cond);
        assert(!_ret);
        if (_ret)
            return false;
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