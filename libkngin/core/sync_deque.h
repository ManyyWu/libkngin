#ifndef _SYNC_DEQUE_H_
#define _SYNC_DEQUE_H_

#include <deque>
#include "define.h"
#include "noncopyable.h"
#include "lock.h"

using std::deque;
using std::iterator;

__NAMESPACE_BEGIN

template <class __T>
class sync_deque : public noncopyable {
public:
    typedef size_t size_type;

protected:
    sync_deque (size_type _s = QUEUE_MAX)
        : m_mutex(NULL), m_cond(NULL), m_max_size(_s)
    {
    }

    virtual
    ~sync_deque ()
    {
        if (m_cond)
            m_cond->release();
        if (m_mutex)
            m_mutex->release();
        this->clear();
    }

public:
    static sync_deque<__T> *
    create (size_type _s, bool _sync)
    {
        sync_deque<__T> *_q = knew(sync_deque, (_s));
        if_not (_q)
            return NULL;
        _q->m_deque.clear();
        if (_sync) {
            _q->m_mutex = mutex::create();
            if_not (_q->m_mutex)
                goto fail;
            _q->m_cond = cond::create(_q->m_mutex);
            if_not (_q->m_cond)
                goto fail;
        }
        return _q;
    fail:
        if (_q->m_cond)
            _q->m_cond->release();
        if (_q->m_mutex)
            _q->m_mutex->release();
        _q->release();
        return NULL;

    }

    virtual void
    release ()
    {
        delete this;
    }

public:
    virtual __T *
    at (size_type _n)
    {
        kassert_r0(_n >= 0 && _n < m_deque.size());
        return (m_deque.empty() ? NULL : m_deque.at(_n));
    }

    virtual __T *
    front ()
    {
        kassert_r0(m_deque.size());
        return (m_deque.empty() ? NULL : m_deque.front());
    }

    virtual __T *
    back ()
    {
        kassert_r0(m_deque.size());
        return (m_deque.empty() ? NULL : m_deque.back());
    }

    virtual __T *
    operator [] (size_type _n)
    {
        kassert_r0(_n >= 0 && _n < m_deque.size());
        return (m_deque.empty() ? NULL : m_deque.at(_n));
    }

public:
    virtual bool
    push_back (__T **_v)
    {
        kassert_r0(_v);
        kassert_r0(*_v);
        if (m_deque.size() > m_max_size)
            return false;
        m_deque.push_back(*_v);
        *_v = NULL;
        return true;
    }

    virtual bool
    push_front (__T **_v)
    {
        kassert_r0(_v);
        if (m_deque.size() > m_max_size)
            return false;

        m_deque.push_front(*_v);
        *_v = NULL;
        return true;
    }

    virtual bool
    pop_back ()
    {
        kassert_r0(m_deque.size());

        m_deque.pop_back();
        return true;
    }

    virtual bool
    pop_front ()
    {
        kassert_r0(m_deque.size());

        m_deque.pop_front();
        return true;
    }

    virtual bool
    insert (size_type _n, __T **_v) // insert before _n
    {
        kassert_r0(_v);
        kassert_r0(_n >= 0 && _n < m_deque.size());
        if (m_deque.size() > m_max_size)
            return false;

        auto iter = m_deque.begin();
        kassert_r0(iter != m_deque.end());
        for (int i = 0; i < _n; i++) {
            kassert_r0(iter++ == m_deque.end());
            ++iter; // ++iter is more efficient
        }
        m_deque.insert(iter, *_v);
        *_v = NULL;
        return true;
    }

    virtual bool
    erase (size_type _n)
    {
        kassert_r0(_n >= 0 && _n < m_deque.size());
        if (m_deque.empty())
            return false;

        auto iter = m_deque.begin();
        kassert_r0(iter != m_deque.end());
        for (int i = 0; i < _n; i++) {
            kassert_r0(iter == m_deque.end());
            ++iter; // ++iter is more efficient
        }
        m_deque.erase(iter);
        return true;
        return false;
    }
    
public:
    virtual void
    shrink ()
    {
        m_deque.shrink_to_fit();
    }

    virtual void
    clear ()
    {
        deque<__T *> _deque;
        m_deque.swap(_deque);
    }

    virtual size_type
    size () const
    {
        return m_deque.size();
    }

    virtual size_type
    max_size () const
    {
        return m_max_size.load();
    }

    virtual void
    set_max_size (size_type _s)
    {
        m_max_size.store(_s);
    }

    virtual bool
    empty () const
    {
        return m_deque.empty();
    }

    virtual bool
    full () const
    {
        return (m_deque.size() >= m_max_size);
    }

public:
    virtual bool
    lock (time_t _ms = TIME_INFINITE)
    {
        kassert_r0(m_mutex && m_cond);

        if (TIME_INFINITE == _ms)
            return m_mutex->lock();
        else
            return m_mutex->timedlock(_ms);
    }

    virtual bool
    trylock ()
    {
        kassert_r0(m_mutex && m_cond);

        return m_mutex->trylock();
    }

    virtual bool
    unlock ()
    {
        kassert_r0(m_mutex && m_cond);

        return m_mutex->unlock();
    }

    virtual bool
    wait (time_t _ms = TIME_INFINITE)
    {
        kassert_r0(m_mutex && m_cond);
        kassert_r0(__time_valid(_ms));

        if (TIME_INFINITE == _ms)
            return m_cond->wait();
        else
            return m_cond->timedwait(_ms);
    }

    virtual bool
    signal ()
    {
        kassert_r0(m_mutex && m_cond);

        return m_cond->signal();
    }
 
    virtual bool
    broadcast ()
    {
        kassert_r0(m_mutex && m_cond);

        return m_cond->broadcast();
    }

protected:
    deque<__T *>           m_deque;

    mutex *                m_mutex;

    cond *                 m_cond;

    std::atomic<size_type> m_max_size;
};

__NAMESPACE_END

#endif /* _SYNC_DEQUE_H_ */
