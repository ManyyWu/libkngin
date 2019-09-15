#ifndef _COND_H_
#define _COND_H_

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif
#include "define.h"
#include "logfile.h"
#include "common.h"
#include "thread.h"
#include "mutex.h"
#include "noncopyable.h"

#ifdef _WIN32
typedef HANDLE         cond_interface;
#else
typedef pthread_cond_t cond_interface;
#endif

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

#ifdef _WIN32
#else
        if (m_cond) {
            _ret = pthread_cond_destroy(m_cond);
            delete m_cond;
        }
#endif
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
#ifdef _WIN32
#else
        _cond_intr = new cond_interface;
        assert(_cond_intr);
        _ret = pthread_cond_init(_cond_intr, NULL);
        if (_ret) {
            delete _cond_intr;
            _cond_intr = NULL;
            return NULL;
        }
        _cond = new cond(_mutex, _cond_intr);
#endif
        assert(_cond);
        return _cond;
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
        int _ret = 0;
#ifdef _WIN32
        assert(m_cond);
#else
        assert(m_cond);
        _ret = pthread_cond_wait(m_cond, m_mutex->get_interface());
        if (_ret)
            return false;
        assert(!_ret);
#endif
        return true;
    }

    inline bool
    timedwait (int _ms)
    {
        int _ret = 0;
#ifdef _WIN32
        assert(m_cond);
#else
        assert(m_cond);
        timespec _ts;
        _ts.tv_sec = _ms / 1000;
        _ts.tv_nsec = (_ms % 1000) * 1000000;
        _ret = pthread_cond_timedwait(m_cond, m_mutex->get_interface(), &_ts);
        if (_ret)
            return false;
        assert(!_ret);
#endif
        return true;
    }

    inline bool
    signal ()
    {
        assert(m_cond);

        int _ret = 0;
#ifdef _WIN32
#else
        assert(m_cond);
        _ret = pthread_cond_signal(m_cond);
        assert(!_ret);
        if (_ret)
            return false;
#endif
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