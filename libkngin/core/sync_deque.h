#ifndef _SYNC_DEQUE_H_
#define _SYNC_DEQUE_H_

#include <deque>
#include "define.h"
#include "noncopyable.h"
#include "lock.h"

__NAMESPACE_BEGIN

template <class __T>
class sync_deque : public noncopyable {
public:
    typedef size_t            size_type;
    typedef std::deque<__T *> deque;

protected:
    sync_deque ()
        : m_mutex(NULL), m_cond(NULL), m_max_size(0)
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
        m_deque.clear();
    }

public:
    static deque *
    create      (size_type _s, bool _sync)
    {
        deque *_q = new deque;
        assert(_q);
        if (!_q)
            return NULL
        _q->deque.clear();
        _q->m_deque.assign(_s, NULL);
        if (_sync) {
            _q->m_mutex = mutex::create();
            assert(_q->mutex);
            if (!_q->m_mutex)
                goto fail;
            _q->cond = cond::create(_q->m_mutex);
            assert(_q->cond);
            if (!_q->cond)
                goto fail;
        }
        return m_q;
    fail:
        delete _q->m_cond;
        delete _q->m_mutex;
        if (_q)
            _q->clear();
        delete _q;
        return NULL;

    }

    virtual void
    release ()
    {
        delete _q->m_cond;
        delete _q->m_mutex;
        if (_q)
            _q->clear();
        delete _q;
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
        if (m_queue.size() >= m_max_size)
            return false;
        m_queue.push_back(_v);
        return true;
    }

    virtual bool
    push_front (__T *_v)
    {
        assert(_v);
        if (m_queue.size() >= m_max_size)
            return false;
        m_queue.push_front(_v);
        return true;
    }

    virtual bool
    pop_back (__T *_v)
    {
        __T *_item = m_queue.empty() ? NULL : m_queue.back();
        m_queue.pop_back();
        return _item;
    }

    virtual bool
    pop_front (__T *_v)
    {
        __T *_item = m_queue.empty() ? NULL : m_queue.front();
        m_queue.pop_front();
        return _item;
    }

    virtual bool
    insert (size_type _n, __T *_v)
    {
        assert(_v);
        if (m_deque.size() >= max_size())
            return false;
        return true;
    }

    virtual bool
    erase (size_type _n)
    {
        assert(_n >= 0 && _n < m_deque.size());
        if (m_deque.empty())
            return false;
        for (std::deque<iterator> iter = m_deque.begin(); iter != iter.end; ++iter)
            if (iter)
                m_deque.erase(iter);
        return true;
    }

    virtual bool
    erase (size_type _f, size_type _n)
    {
        assert(_n >= 0 && _n < m_deque.size());
        if (m_deque.empty())
            return false;
        m_deque.erase(_f, _l);
        return true;
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
        m_deque.size();
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
            return m_mutex->wait();
        else
            return m_cond->timedwait(_ms)
    }

    virtual bool
    signal ()
    {
        assert(m_mutex && m_cond);
        m_cond->signal();
    }
 
    virtual bool
    broadcast ()
    {
        assert(m_mutex && m_cond);
        m_cond->broadcast();
    }

protected:
    std::deque<__T *>      m_deque;

    mutex *                m_mutex;

    cond *                 m_cond;

    std::atomic<size_type> m_max_size;
};

__NAMESPACE_END

#endif /* _SYNC_DEQUE_H_ */
