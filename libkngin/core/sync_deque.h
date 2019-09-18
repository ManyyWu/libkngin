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
    sync_deque (size_type _s = deque<__T *>::max_size())
        : m_mutex(NULL), m_cond(NULL), m_max_size(_s)
    {
        m_deque.clear();
    }

    virtual
    ~sync_deque ()
    {
        if (m_cond)
            m_cond->release();
        if (m_mutex)
            m_mutex->release();
        assert(!this->size());
        this->clear();
    }

public:
    static sync_deque<__T> *
    create (size_type _s, bool _sync)
    {
        sync_deque<__T> *_q = new_nothrow(sync_deque(_s));
        assert(_q);
        if (!_q)
            return NULL;
        _q->m_deque.clear();
        if (_sync) {
            _q->m_mutex = mutex::create();
            assert(_q->m_mutex);
            if (!_q->m_mutex)
                goto fail;
            _q->m_cond = cond::create(_q->m_mutex);
            assert(_q->m_cond);
            if (!_q->m_cond)
                goto fail;
        }
        return _q;
    fail:
        _q->m_cond->release();
        _q->m_mutex->release();
        if (_q)
            _q->clear();
        safe_release(_q);
        return NULL;

    }

    virtual void
    release ()
    {
        m_cond->release();
        m_mutex->release();
        delete this;
    }

public:
    virtual __T *
    at (size_type _n)
    {
        assert(_n >= 0 && _n < m_deque.size());
        return (m_deque.empty() ? NULL : m_deque.at(_n));
    }

    virtual __T *
    front ()
    {
        return (m_deque.empty() ? NULL : m_deque.front());
    }

    virtual __T *
    back ()
    {
        return (m_deque.empty() ? NULL : m_deque.back());
    }

    virtual __T *
    operator [] (size_type _n)
    {
        assert(_n >= 0 && _n < m_deque.size());
        return (m_deque.empty() ? NULL : m_deque.at(_n));
    }

public:
    virtual bool
    push_back (__T *_v)
    {
        assert(_v);
        if (m_deque.size() > m_max_size)
            return false;
        m_deque.push_back(_v);
        return true;
    }

    virtual bool
    push_front (__T *_v)
    {
        assert(_v);
        if (m_deque.size() > m_max_size)
            return false;
        m_deque.push_front(_v);
        return true;
    }

    virtual bool
    pop_back ()
    {
        __T *_item = m_deque.empty() ? NULL : m_deque.back();
        m_deque.pop_back();
        return _item;
    }

    virtual bool
    pop_front ()
    {
        __T *_item = m_deque.empty() ? NULL : m_deque.front();
        m_deque.pop_front();
        return _item;
    }

    virtual bool
    insert (size_type _n, __T *_v)
    {
        assert(_v);
        if (m_deque.size() > max_size())
            return false;
        return true;
    }

    virtual bool
    erase (size_type _n)
    {
        assert(_n >= 0 && _n < m_deque.size());
        if (m_deque.empty())
            return false;
        typename deque<__T *>::iterator iter;
        for (iter = m_deque.begin(); iter != m_deque.end(); ++iter) {
            m_deque.erase(iter);
            return true;
        }
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
        m_deque.clear();
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
    lock (int _ms = INFINITE)
    {
        assert(m_mutex && m_cond);
        if (INFINITE == _ms)
            return m_mutex->lock();
        else
            return m_mutex->timedlock(_ms);
    }

    virtual bool
    trylock (int _ms = INFINITE)
    {
        assert(m_mutex && m_cond);
        return m_mutex->lock();
    }

    virtual bool
    unlock ()
    {
        assert(m_mutex && m_cond);
        return m_mutex->unlock();
    }

    virtual bool
    wait (int _ms = INFINITE)
    {
        assert(m_mutex && m_cond);
        if (INFINITE == _ms)
            return m_cond->wait();
        else
            return m_cond->timedwait(_ms);
    }

    virtual bool
    signal ()
    {
        assert(m_mutex && m_cond);
        return m_cond->signal();
    }
 
    virtual bool
    broadcast ()
    {
        assert(m_mutex && m_cond);
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
