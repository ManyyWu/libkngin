#ifndef _MUTEX_H_
#define _MUTEX_H_

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
typedef HANDLE          mutex_interface;
#else
typedef pthread_mutex_t mutex_interface;
#endif

__NAMESPACE_BEGIN

class mutex : noncopyable {
public:
    mutex () = delete;

protected:
    inline
    mutex (mutex_interface *_mutex_intr)
        : m_mutex(_mutex_intr)
    {
        assert(_mutex_intr);
    }

protected:
    inline
    ~mutex ()
    {
        int _ret = 0;

#ifdef _WIN32
#else
        if (m_mutex) {
            _ret = pthread_mutex_destroy(m_mutex);
            delete m_mutex;
        }
#endif
        assert(!_ret);
    }

public:
    inline static mutex *
    create ()
    {
        int _ret = 0;
        mutex *          _mutex = NULL;
        mutex_interface *_mutex_intr = NULL;
#ifdef _WIN32
#else
        _mutex_intr = new mutex_interface;
        assert(_mutex_intr);
        _ret = pthread_mutex_init(_mutex_intr, NULL);
        if (_ret) {
            delete _mutex_intr;
            _mutex_intr = NULL;
            return NULL;
        }
        _mutex = new mutex(_mutex_intr);
#endif
        assert(_mutex);
        return _mutex;
    }

    inline void
    release ()
    {
        assert(m_mutex);

        delete this;
    }

public:
    inline bool
    lock ()
    {
        int _ret = 0;
#ifdef _WIN32
        assert(m_mutex);
#else
        assert(m_mutex);
        _ret = pthread_mutex_lock(m_mutex);
        if (_ret)
            return false;
        assert(!_ret);
#endif
        return true;
    }

    inline bool
    try_lock ()
    {
        int _ret = 0;
#ifdef _WIN32
        assert(m_mutex);
#else
        assert(m_mutex);
        _ret = pthread_mutex_trylock(m_mutex);
        if (_ret == EBUSY)
            return false;
        assert(!_ret);
#endif
        return true;
    }

    inline bool
    timedlock (int _ms)
    {
        int _ret = 0;
#ifdef _WIN32
        assert(m_mutex);
#else
        assert(m_mutex);
        timespec _ts;
        _ts.tv_sec = _ms / 1000;
        _ts.tv_nsec = (_ms % 1000) * 1000000;
        _ret = pthread_mutex_timedlock(m_mutex, &_ts);
        if (_ret)
            return false;
        assert(!_ret);
#endif
        return true;
    }

    inline bool
    unlock ()
    {
        assert(m_mutex);

        int _ret = 0;
#ifdef _WIN32
#else
        assert(m_mutex);
        _ret = pthread_mutex_unlock(m_mutex);
        assert(!_ret);
        if (_ret)
            return false;
#endif
        return true;
    }

public:
    inline mutex_interface *
    get_interface () const
    {
        return m_mutex;
    }

protected:
    mutex_interface *m_mutex;
};

__NAMESPACE_END

#endif /* _MUTEX_H_ */
