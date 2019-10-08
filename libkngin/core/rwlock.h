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
        kassert(_rwlock_intr);
    }

protected:
    inline
    ~rwlock ()
    {
        int _ret = 0;
        if (m_rwlock)
            _ret = pthread_rwlock_destroy(m_rwlock);
        if_not (!_ret)
            server_fatal("pthread_rwlock_destroy() retturn %d", _ret);
        kdelete(m_rwlock);
    }

public:
    inline static rwlock *
    create ()
    {
        int _ret = 0;
        rwlock * _rwlock = NULL;
        pthread_rwlock_t *_rwlock_intr = NULL;

        knew(_rwlock_intr, pthread_rwlock_t, ());
        if_not (_rwlock_intr)
            return NULL;
        _ret = pthread_rwlock_init(_rwlock_intr, NULL);
        if_not (!_ret) {
            server_fatal("pthread_rwlock_init() return %d", _ret);
            goto fail;
        }
        knew(_rwlock, rwlock, (_rwlock_intr));
        if_not (_rwlock)
            goto fail;
        return _rwlock;
    fail:
        if (_rwlock_intr)
            pthread_rwlock_destroy(_rwlock_intr);
        kdelete(_rwlock_intr);
        kdelete(_rwlock);
        return NULL;
    }

    inline void
    release ()
    {
        kassert(m_rwlock);

        kdelete_this(this);
    }

public:
    inline bool
    rdlock ()
    {
        kassert_r0(m_rwlock);

        int _ret = 0;
        _ret = pthread_rwlock_rdlock(m_rwlock);
        if_not (!_ret) {
            server_fatal("pthread_rwlock_rdlock() return %d", _ret);
            return false;
        }
        return true;
    }

    inline bool
    wrlock ()
    {
        kassert_r0(m_rwlock);

        int _ret = 0;
        _ret = pthread_rwlock_wrlock(m_rwlock);
        if_not (!_ret) {
            server_fatal("pthread_rwlock_wrlock() return %d", _ret);
            return false;
        }
        return true;
    }

    inline bool
    tryrdlock ()
    {
        kassert_r0(m_rwlock);

        int _ret = 0;
        _ret = pthread_rwlock_tryrdlock(m_rwlock);
        if (EBUSY == _ret)
            return false;
        if (!_ret) {
            server_fatal("pthread_rwlock_tryrdlock() return %d", _ret);
            return false;
        }
        return true;
    }

    inline bool
    trywrlock ()
    {
        kassert_r0(m_rwlock);

        int _ret = 0;
        _ret = pthread_rwlock_trywrlock(m_rwlock);
        if (EBUSY == _ret)
            return false;
        if (!_ret) {
            server_fatal("pthread_rwlock_trywrlock() return %d", _ret);
            return false;
        }

        return true;
    }

    inline bool
    timedrdlock (time_t _ms)
    {
        kassert_r0(m_rwlock);
        kassert_r0(__time_valid(_ms));
        if (!__time_valid(_ms))
            return false;

        int _ret = 0;
        timespec _ts;
        timespec_get(&_ts, TIME_UTC);
        _ts.tv_sec += _ms / 1000;
        _ts.tv_nsec += (_ms % 1000) * 1000000;
        _ret = pthread_rwlock_timedrdlock(m_rwlock, &_ts);
        if (ETIMEDOUT == _ret)
            return false;
        if_not (!_ret) {
            server_fatal("pthread_rwlock_timedrdlock(), value = %ld, return %d", _ms, _ret);
            return false;
        }

        return true;
    }

    inline bool
    timedwrlock (time_t _ms)
    {
        kassert_r0(m_rwlock);
        kassert_r0(__time_valid(_ms));
        if (!__time_valid(_ms))
            return false;

        int _ret = 0;
        timespec _ts;
        timespec_get(&_ts, TIME_UTC);
        _ts.tv_sec += _ms / 1000;
        _ts.tv_nsec += (_ms % 1000) * 1000000;
        _ret = pthread_rwlock_timedwrlock(m_rwlock, &_ts);
        if (ETIMEDOUT == _ret)
            return false;
        if_not (!_ret) {
            server_fatal("pthread_rwlock_timedwrlock(), value = %ld, return %d", _ms, _ret);
            return false;
        }

        return true;
    }

    inline bool
    rdunlock ()
    {
        kassert_r0(m_rwlock);

        int _ret = 0;
        _ret = pthread_rwlock_unlock(m_rwlock);
        if (ETIMEDOUT == _ret)
            return false;
        if_not (!_ret) {
            server_fatal("pthread_rwlock_unlock() return %d", _ret);
            return false;
        }

        return true;
    }

    inline bool
    wrunlock ()
    {
        kassert_r0(m_rwlock);

        int _ret = 0;
        _ret = pthread_rwlock_unlock(m_rwlock);
        if (ETIMEDOUT == _ret)
            return false;
        if_not (!_ret) {
            server_fatal("pthread_rwlock_unlock() return %d", _ret);
            return false;
        }

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