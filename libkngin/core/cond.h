#ifndef _COND_H_
#define _COND_H_

#include <pthread.h>
#include "define.h"
#include "logfile.h"
#include "common.h"
#include "thread.h"
#include "mutex.h"
#include "noncopyable.h"

typedef pthread_cond_t cond_interface;

__NAMESPACE_BEGIN

class cond : noncopyable {
public:
    cond () = delete;

protected:
    inline
    cond (mutex *_mutex, cond_interface *_cond_intr)
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
        cond_interface *_cond_intr = NULL;
        _cond_intr = new cond_interface;
        if (_cond_intr)
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
        _ret = pthread_cond_wait(m_cond, m_mutex->get_interface());
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
        _ret = pthread_cond_timedwait(m_cond, m_mutex->get_interface(), &_ts);
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
    inline cond_interface *
    get_interface () const
    {
        return m_cond;
    }

protected:
    cond_interface *m_cond;
    mutex *         m_mutex;
};

__NAMESPACE_END

#endif /* _COND_H_ */