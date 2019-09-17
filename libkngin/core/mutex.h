#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <pthread.h>
#include "define.h"
#include "logfile.h"
#include "common.h"
#include "thread.h"
#include "noncopyable.h"

typedef pthread_mutex_t mutex_interface;

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
        if (m_mutex) {
            pthread_mutex_destroy(m_mutex);
            delete m_mutex;
        }
    }

public:
    inline static mutex *
    create ()
    {
        int _ret = 0;
        mutex * _mutex = NULL;
        mutex_interface *_mutex_intr = NULL;
        _mutex_intr = new mutex_interface;
        if (!_mutex_intr)
            return NULL;
        _ret = pthread_mutex_init(_mutex_intr, NULL);
        if (_ret)
            goto fail;
        _mutex = new mutex(_mutex_intr);
        if (!_mutex)
            goto fail;
        return _mutex;
    fail:
        if (_mutex_intr)
            pthread_mutex_destroy(_mutex_intr);
        delete _mutex_intr;
        _mutex->m_mutex = NULL;
        delete _mutex;
        return NULL;
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
        assert(m_mutex);

        int _ret = 0;
        _ret = pthread_mutex_lock(m_mutex);
        assert(!_ret);
        if (_ret)
            return false;
        return true;
    }

    inline bool
    try_lock ()
    {
        assert(m_mutex);

        int _ret = 0;
        _ret = pthread_mutex_trylock(m_mutex);
        assert(!_ret);
        if (_ret == EBUSY)
            return false;
        return true;
    }

    inline bool
    timedlock (int _ms)
    {
        assert(m_mutex);

        int _ret = 0;
        timespec _ts;
        _ts.tv_sec = _ms / 1000;
        _ts.tv_nsec = (_ms % 1000) * 1000000;
        _ret = pthread_mutex_timedlock(m_mutex, &_ts);
        assert(!_ret);
        if (_ret)
            return false;
        return true;
    }

    inline bool
    unlock ()
    {
        assert(m_mutex);

        int _ret = 0;
        assert(m_mutex);
        _ret = pthread_mutex_unlock(m_mutex);
        assert(!_ret);
        if (_ret)
            return false;
        return true;
    }

public:
    inline const mutex_interface *
    get_interface () const
    {
        return m_mutex;
    }

protected:
    mutex_interface *m_mutex;
};

__NAMESPACE_END

#endif /* _MUTEX_H_ */
