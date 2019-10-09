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
    typedef std::unique_ptr<std::deque<__T *>> sync_queue_ptr;

    typedef size_t                             size_type;

public:
    sync_queue (size_type _s = QUEUE_MAX)
        try
        : m_queue(), m_mutex(), m_cond(&m_mutex), m_max_size(_s)
    {
    } catch (...) {
        log_fatal("sync_queue::sync_queue() error");
        throw;
    }

    virtual
    ~sync_queue ()
    {
        for (auto _iter : m_queue)
            delete _iter;
    }

public:
    virtual __T *
    peek ()
    {
        return (m_queue.empty() ? NULL : m_queue.back());
    }

public:
    virtual bool
    push (__T **_v)
    {
        kassert_r0(_v);
        if (!_v)
            return false;
        if (m_queue.size() > m_max_size)
            return false;
        m_queue.push_front(*_v);
        *_v = NULL;
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
        deque<__T *> _deque;
        m_queue.swap(_deque);
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
    lock (time_t _ms = TIME_INFINITE)
    {
        if_not (__time_valid(_ms))
            return false;

        if (TIME_INFINITE == _ms)
            m_mutex.lock();
        else
            return m_mutex.timedlock(_ms);
        return true;
    }

    virtual bool
    trylock ()
    {

        return m_mutex.trylock();
    }

    virtual void
    unlock ()
    {
        m_mutex.unlock();
    }

    virtual bool
    wait (time_t _ms = TIME_INFINITE)
    {
        kassert_r0(__time_valid(_ms));

        if (TIME_INFINITE == _ms)
            m_cond.wait();
        else
            return m_cond.timedwait(_ms);
        return true;
    }

    virtual void
    signal ()
    {
        signal();
    }
 
    virtual void
    broadcast ()
    {
        m_cond.broadcast();
    }

protected:
    std::deque<__T *>      m_queue;

    mutex                  m_mutex;

    cond                   m_cond;

    std::atomic<size_type> m_max_size;
};

__NAMESPACE_END

#endif /* _SYNC_DEQUE_H_ */