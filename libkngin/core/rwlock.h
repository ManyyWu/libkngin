#ifndef _RWLOCK_H_
#define _RWLOCK_H_

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif
#include "define.h"
#include "logfile.h"
#include "common.h"
#include "thread.h"
#include "noncopyable.h"

#ifdef _WIN32
typedef HANDLE           rwlock_interface;
#else
typedef pthread_rwlock_t rwlock_interface;
#endif

__NAMESPACE_BEGIN

class rwlock : noncopyable {
public:
    rwlock () = delete;

protected:
    inline
    rwlock (rwlock_interface *_rwlock_intr)
    : m_rwlock(_rwlock_intr)
    {
        assert(_rwlock_intr);
    }

protected:
    inline
    ~rwlock ()
    {
        int _ret = 0;

#ifdef _WIN32
#else
        if (m_rwlock) {
            _ret = pthread_rwlock_destroy(m_rwlock);
            delete m_rwlock;
        }
#endif
        assert(!_ret);
    }

public:
    inline static rwlock *
    create ()
    {
        int _ret = 0;
        rwlock *          _rwlock = NULL;
        rwlock_interface *_rwlock_intr = NULL;
#ifdef _WIN32
#else
        _rwlock_intr = new rwlock_interface;
        assert(_rwlock_intr);
        _ret = pthread_rwlock_init(_rwlock_intr, NULL);
        if (_ret) {
            delete _rwlock_intr;
            _rwlock_intr = NULL;
            return NULL;
        }
        _rwlock = new rwlock(_rwlock_intr);
#endif
        assert(_rwlock);
        return _rwlock;
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
        int _ret = 0;
#ifdef _WIN32
        assert(m_rwlock);
#else
        assert(m_rwlock);
        _ret = pthread_rwlock_rdlock(m_rwlock);
        if (_ret)
            return false;
        assert(!_ret);
#endif
        return true;
    }

    inline bool
    wrlock ()
    {
        int _ret = 0;
#ifdef _WIN32
        assert(m_rwlock);
#else
        assert(m_rwlock);
        _ret = pthread_rwlock_wrlock(m_rwlock);
        if (_ret)
            return false;
        assert(!_ret);
#endif
        return true;
    }

    inline bool
    try_rdlock ()
    {
        int _ret = 0;
#ifdef _WIN32
        assert(m_rwlock);
#else
        assert(m_rwlock);
        _ret = pthread_rwlock_tryrdlock(m_rwlock);
        if (_ret == EBUSY)
            return false;
        assert(!_ret);
#endif
        return true;
    }

    inline bool
    try_wrlock ()
    {
        int _ret = 0;
#ifdef _WIN32
        assert(m_rwlock);
#else
        assert(m_rwlock);
        _ret = pthread_rwlock_trywrlock(m_rwlock);
        if (_ret == EBUSY)
            return false;
        assert(!_ret);
#endif
        return true;
    }

    inline bool
    timedrdlock (int _ms)
    {
        int _ret = 0;
#ifdef _WIN32
        assert(m_rwlock);
#else
        assert(m_rwlock);
        timespec _ts;
        _ts.tv_sec = _ms / 1000;
        _ts.tv_nsec = (_ms % 1000) * 1000000;
        _ret = pthread_rwlock_timedrdlock(m_rwlock, &_ts);
        if (_ret)
            return false;
        assert(!_ret);
#endif
        return true;
    }

    inline bool
    timedwrlock (int _ms)
    {
        int _ret = 0;
#ifdef _WIN32
        assert(m_rwlock);
#else
        assert(m_rwlock);
        timespec _ts;
        _ts.tv_sec = _ms / 1000;
        _ts.tv_nsec = (_ms % 1000) * 1000000;
        _ret = pthread_rwlock_timedwrlock(m_rwlock, &_ts);
        if (_ret)
            return false;
        assert(!_ret);
#endif
        return true;
    }

    inline bool
    unlock ()
    {
        assert(m_rwlock);

        int _ret = 0;
#ifdef _WIN32
#else
        assert(m_rwlock);
        _ret = pthread_rwlock_unlock(m_rwlock);
        assert(!_ret);
        if (_ret)
            return false;
#endif
        return true;
    }

public:
    inline rwlock_interface *
    get_interface () const
    {
        return m_rwlock;
    }

protected:
    rwlock_interface *m_rwlock;
};

__NAMESPACE_END

#endif /* _RWLOCK_H_ */