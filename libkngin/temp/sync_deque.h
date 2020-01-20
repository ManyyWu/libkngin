#ifndef KNGIN_SYNC_DEQUE_H
#define KNGIN_SYNC_DEQUE_H

#include <deque>
#include <atomic>
#include <memory>
#include "core/base/define.h"
#include "core/base/logfile.h"
#include "core/base/common.h"
#include "core/base/timestamp.h"
#include "core/base/lock.h"

using std::deque;
using std::iterator;

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/core/base/sync_deque.h"

KNGIN_NAMESPACE_K_BEGIN

template <class Type>
class sync_deque {
public:
    typedef size_t size_type;

public:
    sync_deque ()
        try
        : m_deque(),
          m_mutex(),
          m_cond(&m_mutex),
          m_max_size(KNGIN_QUEUE_MAX)
    {
    } catch (...) {
        log_fatal("sync_deque::sync_deque() error");
        throw;
    }

    explicit
    sync_deque (size_type _s)
        try
        : m_deque(),
          m_mutex(),
          m_cond(&m_mutex),
          m_max_size(_s)
    {
    } catch (...) {
        log_fatal("sync_deque::sync_deque() error");
        throw;
    }

    virtual
    ~sync_deque ()
    {
        for (auto _iter : m_deque)
            delete _iter;
    }

public:
    virtual Type *
    at (size_type _n)
    {
        assert(_n >= 0);
        assert(_n < m_deque.size());
        return (m_deque.empty() ? nullptr : m_deque.at(_n));
    }

    virtual Type *
    front ()
    {
        assert(m_deque.size());
        return (m_deque.empty() ? nullptr : m_deque.front());
    }

    virtual Type *
    back ()
    {
        assert(m_deque.size());
        return (m_deque.empty() ? nullptr : m_deque.back());
    }

    virtual Type *
    operator [] (size_type _n)
    {
        assert(_n >= 0);
        assert(_n < m_deque.size());
        return (m_deque.empty() ? nullptr : m_deque.at(_n));
    }

public:
    virtual bool
    push_back (Type **_v)
    {
        assert(_v);
        assert(*_v);
        if (m_deque.size() > m_max_size)
            return false;
        m_deque.push_back(*_v);
        *_v = nullptr;
        return true;
    }

    virtual bool
    push_front (Type **_v)
    {
        assert(_v);
        if (m_deque.size() > m_max_size)
            return false;

        m_deque.push_front(*_v);
        *_v = nullptr;
        return true;
    }

    virtual bool
    pop_back ()
    {
        assert(m_deque.size());

        m_deque.pop_back();
        return true;
    }

    virtual bool
    pop_front ()
    {
        assert(m_deque.size());

        m_deque.pop_front();
        return true;
    }

    virtual bool
    insert (size_type _n, Type **_v) // insert before _n
    {
        assert(_v);
        assert(_n >= 0);
        assert(_n < m_deque.size());
        if (m_deque.size() > m_max_size)
            return false;

        auto iter = m_deque.begin();
        assert(iter != m_deque.end());
        for (size_type i = 0; i < _n; ++i) {
            assert(iter++ == m_deque.end());
            ++iter;
        }
        m_deque.insert(iter, *_v);
        *_v = nullptr;
        return true;
    }

    virtual bool
    erase (size_type _n)
    {
        assert(_n >= 0);
        assert(_n < m_deque.size());
        if (m_deque.empty())
            return false;

        auto iter = m_deque.begin();
        assert(iter != m_deque.end());
        for (size_type i = 0; i < _n; ++i) {
            assert(iter == m_deque.end());
            ++iter;
        }
        m_deque.erase(iter);
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
        deque<Type *> _deque;
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
        return m_deque.empty();
    }

    virtual bool
    full () const
    {
        return (m_deque.size() >= m_max_size);
    }

public:
    virtual bool
    lock (timestamp _ms = timestamp::infinite())
    {
        if (_ms == timestamp::infinite())
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
        if (_ms == timestamp::infinite())
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
    std::deque<Type *> m_deque;

    mutex              m_mutex;

    cond               m_cond;

    std::atomic_size_t m_max_size;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_SYNC_DEQUE_H */
