#ifndef _RWLOCK_H_
#define _RWLOCK_H_

#ifdef _WIN32
#else
#include <pthread.h>
#include "pthread.h"
#endif
#include "define.h"
#include "logfile.h"
#include "common.h"
#include "thread.h"
#include "noncopyable.h"

__NAMESPACE_BEGIN

class rwlock : noncopyable {
public:
    rwlock () = delete;

protected:
    inline
    rwlock (pthread_rwlock_t *_rwlock_intr)
        : m_rwlock(_rwlock_intr)
    {
        assert(_rwlock_intr);
    }

protected:
    inline
    ~rwlock ()
    {
        if (m_rwlock)
            pthread_rwlock_destroy(m_rwlock);
        safe_release(m_rwlock);
    }

public:
    inline static rwlock *
    create ()
    {
        int _ret = 0;
        rwlock * _rwlock = NULL;
        pthread_rwlock_t *_rwlock_intr = NULL;

        _rwlock_intr = new_nothrow(pthread_rwlock_t);
        if (!_rwlock_intr)
            return NULL;
        _ret = pthread_rwlock_init(_rwlock_intr, NULL);
        if (_ret)
            goto fail;
        _rwlock = new_nothrow(rwlock(_rwlock_intr));
        if (!_rwlock)
            goto fail;
        return _rwlock;
    fail:
        if (_rwlock_intr)
            pthread_rwlock_destroy(_rwlock_intr);
        safe_release(_rwlock_intr);
        _rwlock->m_rwlock = NULL;
        safe_release(_rwlock);
        return NULL;
    }

    inline void
    release ()
    {
        assert(m_rwlock);
        delete this;
    }

public:
    inline bool
    rdlock ()
    {
        assert(m_rwlock);

        int _ret = 0;
        _ret = pthread_rwlock_rdlock(m_rwlock);
        assert(!_ret);
        if (_ret)
            return false;
        return true;
    }

    inline bool
    wrlock ()
    {
        assert(m_rwlock);

        int _ret = 0;
        _ret = pthread_rwlock_wrlock(m_rwlock);
        assert(!_ret);
        if (_ret)
            return false;
        return true;
    }

    inline bool
    try_rdlock ()
    {
        assert(m_rwlock);

        int _ret = 0;
        _ret = pthread_rwlock_tryrdlock(m_rwlock);
        assert(!_ret);
        if (_ret == EBUSY)
            return false;
        return true;
    }

    inline bool
    try_wrlock ()
    {
        assert(m_rwlock);

        int _ret = 0;
        _ret = pthread_rwlock_trywrlock(m_rwlock);
        assert(!_ret);
        if (_ret == EBUSY)
            return false;
        return true;
    }

    inline bool
    timedrdlock (int _ms)
    {
        assert(m_rwlock);

        int _ret = 0;
        timespec _ts;
        _ts.tv_sec = _ms / 1000;
        _ts.tv_nsec = (_ms % 1000) * 1000000;
        _ret = pthread_rwlock_timedrdlock(m_rwlock, &_ts);
        assert(!_ret);
        if (_ret)
            return false;
        return true;
    }

    inline bool
    timedwrlock (int _ms)
    {
        int _ret = 0;
        assert(m_rwlock);
        timespec _ts;
        _ts.tv_sec = _ms / 1000;
        _ts.tv_nsec = (_ms % 1000) * 1000000;
        _ret = pthread_rwlock_timedwrlock(m_rwlock, &_ts);
        assert(!_ret);
        if (_ret)
            return false;
        return true;
    }

    inline bool
    rdunlock ()
    {
        assert(m_rwlock);

        int _ret = 0;
        _ret = pthread_rwlock_unlock(m_rwlock);
        assert(!_ret);
        if (_ret)
            return false;
        return true;
    }

    inline bool
    wrunlock ()
    {
        assert(m_rwlock);

        int _ret = 0;
        _ret = pthread_rwlock_unlock(m_rwlock);
        assert(!_ret);
        if (_ret)
            return false;
        return true;
    }

public:
    inline const pthread_rwlock_t *
    get_interface () const
    {
        return m_rwlock;
    }

protected:
    pthread_rwlock_t *m_rwlock;
};

__NAMESPACE_END

#endif /* _RWLOCK_H_ */