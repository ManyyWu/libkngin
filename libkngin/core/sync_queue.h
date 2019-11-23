#ifndef _SYNC_QUEUE_H_
#define _SYNC_QUEUE_H_

#include <queue>
#include <atomic>
#include <memory>
#include "define.h"
#include "logfile.h"
#include "common.h"
#include "timestamp.h"
#include "lock.h"

using std::deque;

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/core/sync_queue.h"

KNGIN_NAMESPACE_K_BEGIN

template <class __T>
class sync_queue {
public:
    typedef std::unique_ptr<std::deque<__T *>> sync_queue_ptr;

    typedef size_t                             size_type;

public:
    sync_queue ()
        try
        : m_queue(), m_mutex(), m_cond(&m_mutex), m_max_size(QUEUE_MAX)
    {
    } catch (...) {
        log_fatal("sync_queue::sync_queue() error");
        throw;
    }


    explicit
    sync_queue (size_type _s)
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
        return (m_queue.empty() ? nullptr : m_queue.back());
    }

public:
    virtual bool
    push (__T **_v)
    {
        check(_v);
        if (!_v)
            return false;
        if (m_queue.size() > m_max_size)
            return false;
        m_queue.push_front(*_v);
        *_v = nullptr;
        return true;
    }

    virtual __T *
    pop ()
    {
        __T *_item = m_queue.empty() ? nullptr : m_queue.back();
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
        return m_max_size;
    }

    virtual void
    set_max_size (size_type _s)
    {
        m_max_size = _s;
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
    lock (timestamp _ms = timestamp::infinite())
    {
        if (timestamp::infinite() == _ms)
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
    wait (timestamp _ms = timestamp::infinite())
    {
        if (timestamp::infinite() == _ms)
            m_cond.wait();
        else
            return m_cond.timedwait(_ms);
        return true;
    }

    virtual void
    signal ()
    {
        m_cond.signal();
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

KNGIN_NAMESPACE_K_END

#endif /* _SYNC_DEQUE_H_ */