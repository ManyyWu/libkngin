#ifndef _SYNC_QUEUE_H_
#define _SYNC_QUEUE_H_

#include <queue>
#include "define.h"
#include "noncopyable.h"
#include "lock.h"

using std::deque;

__NAMESPACE_BEGIN

template <class __T>
class sync_queue : public noncopyable {
public:
    typedef size_t size_type;

protected:
    sync_queue (size_type _s = deque<__T *> max_size()) 
        : m_mutex(NULL), m_cond(NULL), m_max_size(_s)
    {
        m_queue.clear();
    }

    virtual
    ~sync_queue ()
    {
        if (m_cond)
            m_cond->release();
        if (m_mutex)
            m_mutex->release();
        assert(!this->size());
        this->clear();
    }

public:
    static sync_queue<__T> *
    create (size_type _s, bool _sync)
    {
        sync_queue<__T> *_q = new sync_queue(_s);
        assert(_q);
        if (!_q)
            return NULL;
        _q->m_queue.clear();
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
        delete _q;
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
    peek ()
    {
        return (m_queue.empty() ? NULL : m_queue.back());
    }

public:
    virtual bool
    push (__T *_v)
    {
        assert(_v);
        if (m_queue.size() > m_max_size)
            return false;
        m_queue.push_front(_v);
        return true;
    }

    virtual __T *
    pop ()
    {
        __T *_item = m_queue.empty() ? NULL : m_queue.back();
        m_queue.pop_back();
        return _item;
    }

public:
    virtual void
    shrink ()
    {
        m_queue.shrink_to_fit();
    }

    virtual void
    clear ()
    {
        m_queue.clear();
    }

    virtual size_type
    size () const
    {
        return m_queue.size();
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
        return m_queue.empty();
    }

    virtual bool
    full () const
    {
        return (m_queue.size() >= m_max_size);
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
    broadcaset ()
    {
        assert(m_mutex && m_cond);
        return m_cond->broadcast();
    }

protected:
    std::deque<__T *>      m_queue;

    mutex *                m_mutex;

    cond *                 m_cond;

    std::atomic<size_type> m_max_size;
};

__NAMESPACE_END

#endif /* _SYNC_DEQUE_H_ */